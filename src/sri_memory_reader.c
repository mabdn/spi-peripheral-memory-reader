#include <stdio.h>
#include <stdbool.h>
#include "component.h"
#include "address_iterator.h"

#define BYTE_SIZE 8
#define MAX_READ_ATTEMPTS 5
#define READ_COMMAND_BYTE 0x55
#define READ_COMMAND_PACKET_LENGTH 2 // in bytes
#define DATA_RESPONSE_PACKET_LENGTH 1 // in bytes
#define MSG_LOG_DATA_TRANSFER_SUCCESS "Read address 0x%02x: 0x%02x (no error)\n"
#define MSG_LOG_DATA_TRANSFER_FAILED "Data invalid while reading 0x%02x\n for the %d. time. Received data: 0x%02x\n"
#define DATA_OUTPUT_FORMAT "%c"

// Functions private / local to this file

bool is_data_valid(unsigned char data);
unsigned char read_command(unsigned char address);

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


    read_command(0x00);
    read_command(0x81);
    read_command(0x31);

    return 0;
    
}

int read_memory_bytewise(int min_cycle_time, FILE *output_stream, bool use_log, FILE *log_stream)
{
    unsigned char address;
    unsigned char data;

    while (address_iterator_next(&address))
    {

        // Read data
        for (int retries = 0; retries < MAX_READ_ATTEMPTS; retries++)
        {
            data = read_command(address); 

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

            //TODO Wait until next cycle

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

unsigned char read_command(unsigned char address)
{
    int send_mosi = address << BYTE_SIZE | READ_COMMAND_BYTE;
    int received_miso = 0;

    int sent_mosi = 0; // TODO debug only

    for (int i = 0; i < READ_COMMAND_PACKET_LENGTH * BYTE_SIZE; i++)
    {
        printf(" --- Cycle %d ---\n", i);
        SET_CLK(true);

        bool mosi_bit = (send_mosi >> i) & 1;
        printf("mosi_bit: %d\n", mosi_bit);
        SET_MOSI(mosi_bit);
        sent_mosi = sent_mosi | (mosi_bit << i);




        SET_CLK(false);
    }
    for (int i = 0; i < DATA_RESPONSE_PACKET_LENGTH * BYTE_SIZE; i++)
    {
        SET_CLK(true);
        long miso_bit = READ_MISO();
        printf("miso_bit: %d\n", miso_bit);
        received_miso = received_miso | (miso_bit << i);
        SET_CLK(false);
    }

    printf("sent_mosi    : %016lx\n", sent_mosi);
    printf("received_miso: %016lx\n", received_miso);
    
    return received_miso;
}