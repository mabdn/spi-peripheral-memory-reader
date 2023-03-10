#define _GNU_SOURCE // Sets POSIX test feature macro to get clock_nanosleep() and CLOCK_MONOTONIC from time.h
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "component.h"
#include "address_iterator.h"
#include "spi_memory_reader.h"

#define BYTE_SIZE 8
#define MAX_READ_ATTEMPTS 5
#define READ_COMMAND_BYTE 0xAA
#define READ_COMMAND_PACKET_LENGTH 2 * BYTE_SIZE
#define DATA_RESPONSE_PACKET_LENGTH 1 * BYTE_SIZE

#define kHz_TO_ns(frequency) 1000000 / frequency

#define MSG_LOG_DATA_TRANSFER_SUCCESS "Read address 0x%02x: 0x%02x (no error)\n"
#define MSG_LOG_DATA_TRANSFER_FAILED "Data invalid while reading 0x%02x for the %d. time. Received data: 0x%02x\n"
#define MSG_ERROR_OPENING_OUTPUT_FILE "Error opening output file. Aborting.\n"
#define MSG_ERROR_OPENING_LOG_FILE "Error opening log file. Aborting.\n"
#define DATA_OUTPUT_FORMAT "%c"
#define PATH_LOG_FILE "read_cots_memory_via_spi_app.log"

// Functions private / local to this file

bool is_data_valid(unsigned char data);
unsigned char read_command(unsigned char address, struct timespec half_cycle_time);

int main()
{
    int frequency = 1000; // in kHz

    // Calculate cycle time in nano seconds
    struct timespec min_cycle_time = {0, kHz_TO_ns(frequency)};

    FILE *output_stream = fopen("out.txt", "w");
    FILE *log_stream = fopen(PATH_LOG_FILE, "w");

    if (output_stream == NULL)
    {
        fprintf(stderr, MSG_ERROR_OPENING_OUTPUT_FILE);
        exit(-1);
    }
    if (log_stream == NULL)
    {
        fprintf(stderr, MSG_ERROR_OPENING_LOG_FILE);
        exit(-1);
    }


    read_memory_bytewise(min_cycle_time, output_stream, true, log_stream);

    return 0;

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
    read_command(0xA1, half_cycle_time);
    read_command(0x00, half_cycle_time);
    read_command(0x81, half_cycle_time);
    read_command(0x31, half_cycle_time);
    read_command(0x00, half_cycle_time);
    read_command(0x81, half_cycle_time);

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
        data = data >> 1; // Remove parity bit which is located in the LSB
        fprintf(output_stream, DATA_OUTPUT_FORMAT, data); // TODO insert line breaks if necessary
    }

    return 0;
}

/*
Time Complexity: The time taken by above algorithm is proportional to the number of bits set.
Realistic time complexity is O(number of bits set in data). 
Thus, worst case complexity is w.r.t to word length l of data is O(l). 
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

/* sleep command and loops could be rewritten in a way to avoid code duplication with method calls.
However, I decided to accept the small amounto of code duplication here to achieve better efficiency.
This code is at the heart of the program's communication with the peripheral device
and should be as efficient as possible. */
unsigned char read_command(unsigned char address, struct timespec half_cycle_time)
{
    int send_mosi = READ_COMMAND_BYTE << BYTE_SIZE | address;
    unsigned char received_miso = 0;

    int debug_mosi = 0;
    int debug_miso = 0;
    int packet_length_minus_1 = READ_COMMAND_PACKET_LENGTH - 1; // Pre-calculate for efficiency

    for (int i = 0; i < READ_COMMAND_PACKET_LENGTH; i++)
    {
        // Write data to MOSI MSB first, LSB last
        bool mosi_bit = (send_mosi >> (packet_length_minus_1 - i)) & 1; 

        SET_CLK(true);
        SET_MOSI(mosi_bit);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &half_cycle_time, NULL);
        SET_CLK(false);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &half_cycle_time, NULL);
    }
    // Unset to avoid floating MOSI pin and thus preventing the peripheral device from answering
    SET_MOSI(false);

    for (int i = 0; i < DATA_RESPONSE_PACKET_LENGTH; i++)
    {
        SET_CLK(true);
        bool miso_bit = READ_MISO();
        clock_nanosleep(CLOCK_MONOTONIC, 0, &half_cycle_time, NULL);
        SET_CLK(false);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &half_cycle_time, NULL);

        // Data is received MSB first, LSB last
        received_miso = received_miso << 1;
        received_miso = received_miso | miso_bit;
    }

    return received_miso;
}