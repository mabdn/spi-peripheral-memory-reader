#define _GNU_SOURCE // Sets POSIX test feature macro to get support for time.h
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "spi_memory_reader.h"

#define kHz_TO_ns(frequency) 1000000 / frequency

#define MSG_GET_FREQUENCY "Enter frequency to use for SPI communication with COTS component:\n(in kHz; -1 for communication as fast as program can run):\n"
#define MSG_GET_OUTPUT_FILE_NAME "Enter output file name (max. 63 characters):\n"
#define MSG_STARTING_READING "Reading memory from COTS compenent with cycle time of %ldns and writing to file '%s'.\n"
#define MSG_ERROR_OPENING_OUTPUT_FILE "Error opening output file. Aborting.\n"
#define MSG_ERROR_OPENING_LOG_FILE "Error opening log file. Aborting.\n"

#define PATH_LOG_FILE "read_cots_memory_via_spi_app.log"

/*
 * Main entry point of the application. Interacts with the user and calls
 * the actual application logic.
 * 
 * Can be seen as the view and controller in the MVC pattern. The model
 * are all remaining files.
 * 
 * The application prompts the user for a frequency to interact with
 * the peripheral device and name of an output file. 
 * 
 *  - The provided frequency is the frequency the application will use
 *    for serial communication with the peripheral device. 
 *    More precisely, it defines the clock frequency to use for the 
 *    Serial Peripheral Interface (SPI) protocol.
 *
 *  - The provided file specifies a path to where the application should
 *    write the read memory content.  
 * 
 * Additionally, the application will create a log file in the same
 * directory as the executable. The log file will contain information
 * about the data transfer.
 * 
 * Assumes that the user cooperates. The user could exploit the program by
 * entering malicious a frequency or file name! Security vulnerabilities 
 * with user interaction are no concern for this project, though.
 * 
 * @return 0 on success, -1 on error.
 */
int main()
{
    // Ask user for frequency in kHz
    printf(MSG_GET_FREQUENCY);
    int frequency;
    scanf("%d", &frequency);

    // Calculate cycle time in nano seconds if frequency is valid. Else use cycle time 0
    struct timespec min_cycle_time = {0, 0};
    if (frequency > 0)
    {
        min_cycle_time.tv_nsec = kHz_TO_ns(frequency);
    }

    // Ask user for output file name
    printf(MSG_GET_OUTPUT_FILE_NAME);
    char output_file_name[64];
    scanf("%63s", output_file_name);
    
    FILE *output_stream = fopen(output_file_name, "w");
    FILE *log_stream = fopen(PATH_LOG_FILE, "w");
    if (output_stream == NULL)
    {
        fprintf(stderr, MSG_ERROR_OPENING_OUTPUT_FILE);
        return -1;
    }
    if (log_stream == NULL)
    {
        fprintf(stderr, MSG_ERROR_OPENING_LOG_FILE);
        return -1;
    }

    // Execute application logic
    printf(MSG_STARTING_READING, min_cycle_time.tv_nsec, output_file_name);
    read_memory_bytewise(min_cycle_time, output_stream, true, log_stream);

    return 0;
}