#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "component.h"
#include "address_iterator.h"

#define BYTE_SIZE 8
#define MAX_READ_ATTEMPTS 5
#define READ_COMMAND_BYTE 0x55
#define READ_COMMAND_PACKET_LENGTH 2 * BYTE_SIZE
#define DATA_RESPONSE_PACKET_LENGTH 1 * BYTE_SIZE
#define MSG_LOG_DATA_TRANSFER_SUCCESS "Read address 0x%02x: 0x%02x (no error)\n"
#define MSG_LOG_DATA_TRANSFER_FAILED "Data invalid while reading 0x%02x\n for the %d. time. Received data: 0x%02x\n"
#define DATA_OUTPUT_FORMAT "%c"

// Functions private / local to this file

bool is_data_valid(unsigned char data);
unsigned char read_command(unsigned char address, struct timespec half_cycle_time);

int main()
{
    printf("Hello World !\n");
    int a = sizeof(unsigned char);
    int alpha = true;
    while (alpha)
    {
        alpha = address_iterator_next(&a);
    }
    int b = is_data_valid(0x00);
    int c = is_data_valid(0x01);
    int d = is_data_valid(0x7b);
    int e = is_data_valid(0xa3);

    struct timespec half_cycle_time = {0, 1000 / 2};

    read_command(0x00, half_cycle_time);
    read_command(0x81, half_cycle_time);
    read_command(0x31, half_cycle_time);

    return 0;
}

int read_memory_bytewise(struct timespec min_cycle_time, FILE *output_stream, bool use_log, FILE *log_stream)
{
    unsigned char address;
    unsigned char data;
    struct timespec half_cycle_time = {min_cycle_time.tv_sec / 2, min_cycle_time.tv_nsec / 2};
    
    while (address_iterator_next(&address))
    {
        // Read data
        for (int retries = 0; retries < MAX_READ_ATTEMPTS; retries++)
        {
            data = read_command(address, half_cycle_time);

            if (is_data_valid(data))
            {
                // Log success
                if (use_log)
                    fprintf(log_stream, MSG_LOG_DATA_TRANSFER_SUCCESS, address, data);
                break;
            }
            else
            {
                // Log failure and retry
                if (use_log)
                    fprintf(log_stream, MSG_LOG_DATA_TRANSFER_FAILED, address, retries, data);
            }
        }

        // Output data
        fprintf(output_stream, DATA_OUTPUT_FORMAT, data); // TODO insert line breaks if necessary
    }
}

/*
Time Complexity: The time taken by above algorithm is proportional to the number of bits set. Worst case complexity is O(Log n).
Auxiliary Space: O(1)
*/
bool is_data_valid(unsigned char data)
{
    // Start with uneven parity to get 1 as result <=> even parity in data
    bool parity = 1;

    // While there is a bit set anywhere in the data
    while (data)
    {
        // Invert parity
        parity = !parity;
        // Unset the rightmost bit
        data = data & (data - 1);
    }
    return parity;
}

unsigned char read_command(unsigned char address, struct timespec half_cycle_time)
{
    int send_mosi = address << BYTE_SIZE | READ_COMMAND_BYTE;
    int received_miso = 0;

    for (int i = 0; i < READ_COMMAND_PACKET_LENGTH; i++)
    {
        bool mosi_bit = (send_mosi >> i) & 1;

        SET_CLK(true);
        SET_MOSI(mosi_bit);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &half_cycle_time, NULL);
        SET_CLK(false);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &half_cycle_time, NULL);
    }

    for (int i = 0; i < DATA_RESPONSE_PACKET_LENGTH; i++)
    {
        SET_CLK(true);
        bool miso_bit = READ_MISO();
        clock_nanosleep(CLOCK_MONOTONIC, 0, &half_cycle_time, NULL);
        SET_CLK(false);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &half_cycle_time, NULL);

        received_miso = received_miso | (miso_bit << i);
    }

    printf("sent_mosi    : %04x\n", send_mosi);
    printf("received_miso: %02x\n", received_miso);

    return received_miso;
}