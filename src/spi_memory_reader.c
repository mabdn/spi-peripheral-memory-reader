#define _GNU_SOURCE // Sets POSIX test feature macro to get support for time.h
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "component.h"
#include "address_iterator.h"
#include "spi_memory_reader.h"

#define BYTE_SIZE 8
#define MAX_READ_ATTEMPTS 5
#define READ_COMMAND_BYTE 0xAA
#define READ_COMMAND_PACKET_LENGTH 2 * BYTE_SIZE
#define DATA_RESPONSE_PACKET_LENGTH 1 * BYTE_SIZE

#define MSG_LOG_DATA_TRANSFER_SUCCESS "Read address 0x%02x: 0x%02x (no error)\n"
#define MSG_LOG_DATA_TRANSFER_FAILED "Data invalid while reading 0x%02x for the %d. time. Received data: 0x%02x\n"
#define DATA_OUTPUT_FORMAT "%c"

/* -- Functions private/local to this file -- */

bool is_data_valid(uint8_t data);
uint8_t read_command(uint8_t address, struct timespec half_cycle_time);


/* -- Implementation -- */

// Documentation in header file
int read_memory_bytewise(struct timespec min_cycle_time, FILE *output_stream, bool use_log, FILE *log_stream)
{
    uint8_t address;
    uint8_t data;
    struct timespec half_cycle_time = {min_cycle_time.tv_sec / 2, min_cycle_time.tv_nsec / 2};

    while (address_iterator_next(&address))
    {
        // Read data
        for (int retries = 0; retries < MAX_READ_ATTEMPTS; retries++)
        {
            data = read_command(address, half_cycle_time);

            if (is_data_valid(data))
            {
                // Log success and next
                if (use_log) fprintf(log_stream, MSG_LOG_DATA_TRANSFER_SUCCESS, address, data);
                break;
            }
            else
            {
                // Log failure and retry
                if (use_log) fprintf(log_stream, MSG_LOG_DATA_TRANSFER_FAILED, address, retries, data);
            }
        }

        // Output data
        data = data >> 1; // Remove parity bit which is located in the LSB
        fprintf(output_stream, DATA_OUTPUT_FORMAT, data);
    }

    return 0;
}

/*
 * Checks whether a given data byte is valid.
 *
 * This method encapsulates the logic to determine whether a data byte is valid. In this case,
 * it is a parity check. The data is valid if the number of bits set is even.
 *
 * Time Complexity: The time taken by the algorithm is proportional to the number of bits set.
 * Realistic time complexity is O(number of bits set in data).
 * Thus, worst case complexity w.r.t to word length L of data is O(L).
 * Auxiliary Space: O(1)
 *
 * @param data The data byte to check for validity.
 * @return True if the data is valid, false otherwise.
 */
bool is_data_valid(uint8_t data)
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

/*
 * Executes the read command via SPI by sending the necessary bit-level signals to the device.
 *
 * This method encapsulates the logic of the SPI protocol.
 *
 * Sleep commands and loops could be rewritten by delegating them to method calls
 * to avoid code duplication. However, I decided to accept the small amount of code duplication here
 * to achieve better efficiency.
 * This code is at the heart of the program's communication with the peripheral device
 * and should be as efficient as possible.
 *
 * @param address The peripheral device's memory address to read data from.
 * @param half_cycle_time The time between two consecutive SPI signal edges.
 * @return The byte of data read from the peripheral device's memory.
 */
uint8_t read_command(uint8_t address, struct timespec half_cycle_time)
{
    int send_mosi = READ_COMMAND_BYTE << BYTE_SIZE | address;
    uint8_t received_miso = 0;

    int packet_length_minus_1 = READ_COMMAND_PACKET_LENGTH - 1; // Pre-calculate for efficiency

    for (int i = 0; i < READ_COMMAND_PACKET_LENGTH; i++)
    {
        // Write data to MOSI MSB first, LSB last
        // Get bit at position i of send_mosi starting from MSB
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