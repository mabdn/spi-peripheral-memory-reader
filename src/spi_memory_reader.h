#include <stdio.h>
#include <stdbool.h>
#include <time.h>

/*
 * Template method pattern for reading a data from certain part of memory of
 * any peripheral device.
 * 
 * Also includes a validity check of the data before writing it to the output stream.
 * Retries reading the data if the validity check fails.
 *
 * Note that this method does not need to know the concrete protocol, the validity check
 * mechanism, nor which addresses to read. These are abstracted away by delegating method calls.
 * This makes the method understandable and maintainable. 
 * 
 * If efficiency was our main concern, we should inline the delegating method calls. An 
 * optimizing compiler will do this for us anyway. Given the overhead of the file I/O of 
 * writing the data to the output stream, the overhead of the method calls probably
 * negligible here.
 *
 * @param min_cycle_time The minimum time between two consecutive SPI cycles.
 * @param output_stream The stream to which the read data is written.
 * @param use_log If true, the function will log the data transfer to the log_stream.
 * @param log_stream The stream to which the log messages are written.
 * @return 0 on success, -1 on error.
 */
int read_memory_bytewise(struct timespec min_cycle_time, FILE *output_stream, bool use_log,
                         FILE *log_stream);