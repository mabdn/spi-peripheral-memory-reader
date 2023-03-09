#include <stdio.h>
#include "component.h"
#define BYTE_SIZE 8
#define MAX_READ_ATTEMPTS 5
#define MSG_LOG_DATA_TRANSFER_SUCCESS "Read address 0x%02x: 0x%02x (no error)\n"
#define MSG_LOG_DATA_TRANSFER_FAILED "Data invalid while reading 0x%02x\n for the %d. time. Received data: 0x%02x\n"
#define DATA_OUTPUT_FORMAT "%c"

int main()
{
    printf("Hello World !\n");
    int a = sizeof(long);
    SET_CLK(true);
    unsigned char read_command = 0x55;
    unsigned char address = 0x31;
    unsigned char tail = 0x00;
    long packet = tail << 2 * BYTE_SIZE | address << BYTE_SIZE | read_command;
    packet = 0x003155003155;
    long sent_mosi = 0;
    long received_miso = 0;

    SET_CLK(false);
    for (int i = 0; i < 8 * BYTE_SIZE; i++)
    {
        printf("--- Cycle %d ---\n", i);

        long mosi_bit = (packet >> i) & 1;
        printf("mosi_bit: %d\n", mosi_bit);
        SET_MOSI(mosi_bit);
        sent_mosi = sent_mosi | (mosi_bit << i);

        SET_CLK(true);

        long miso_bit = READ_MISO();
        printf("miso_bit: %d\n", miso_bit);
        received_miso = received_miso | (miso_bit << i);

        SET_CLK(false);
    }
    printf("sent_mosi    : %016lx\n", sent_mosi);
    printf("received_miso: %016lx\n", received_miso);
}

int read_memory_bytewise(int min_cycle_time, FILE *output_stream, FILE *log_stream)
{
    unsigned char address;
    unsigned char data;

    while ((address = address_iterator_next()) >= 0)
    {

        // Read data
        for (int retries = 0; retries < MAX_READ_ATTEMPTS; retries++)
        {
            data = read_command(address);

            if (is_data_valid(data))
            {
                // Log success
                fprintf(log_stream, MSG_LOG_DATA_TRANSFER_SUCCESS, address, data);
                break;
            }
            else
            {
                // Log failure and retry
                fprintf(log_stream, MSG_LOG_DATA_TRANSFER_FAILED, address, retries, data);
            }
        }

        // Output data
        fprintf(output_stream, DATA_OUTPUT_FORMAT, data); // TODO insert line breaks if necessary
    }
}

// Variant of read_memory_bytewise() that does not use logs. This is code duplication but sensible in this case to improve performance.
int read_memory_bytewise(int min_cycle_time, FILE *output_stream)
{
    unsigned char address;
    unsigned char data;

    while ((address = address_iterator_next()) >= 0)
    { // TODO need to hand address iterator with pointer?

        // Read data
        for (int retries = 0; retries < MAX_READ_ATTEMPTS; retries++)
        {
            data = read_command(address);

            if (is_data_valid(data))
            {
                break;
            }
        }

        // Output data
        fprintf(output_stream, DATA_OUTPUT_FORMAT, data); // TODO insert line breaks if necessary
    }
}
