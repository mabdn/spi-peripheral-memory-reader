#define _GNU_SOURCE // Sets POSIX test feature macro to get support for time.h
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "spi_memory_reader.h"

#define kHz_TO_ns(frequency) 1000000 / frequency

#define MSG_GET_FREQUENCY "Enter frequency to use for SPI communication with COTS component:\n (in kHz; -1 for communication as fast as program can run): "
#define MSG_ERROR_OPENING_OUTPUT_FILE "Error opening output file. Aborting.\n"
#define MSG_ERROR_OPENING_LOG_FILE "Error opening log file. Aborting.\n"

#define PATH_LOG_FILE "read_cots_memory_via_spi_app.log"

int main()
{
    // Ask user for frequency in kHz
    int frequency;
    printf(MSG_GET_FREQUENCY);
    scanf("%d", &frequency);

    // Calculate cycle time in nano seconds if frequency is valid. Else use cycle time 0
    struct timespec min_cycle_time = {0, 0};
    if (frequency > 0)
    {
        min_cycle_time.tv_nsec = kHz_TO_ns(frequency);
    }

    // Ask user for output file name
    // TODO use safe scanf
    

    FILE *output_stream = fopen("out.txt", "w");
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

    read_memory_bytewise(min_cycle_time, output_stream, true, log_stream);

    return 0;
}