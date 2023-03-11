#include <stdbool.h>
#include <stdint.h>

/*
 * Provides an iterator over the addresses of the memory which the
 * application should read data from.
 *
 * Thus, this method encapsulates the knowledge of which memory addresses
 * the application is supposed to read. In this case, it is all
 * memory addresses in order from 0x00 to 0xFF.
 *
 * @param address A pointer to a variable which the next memory address
 *                will be written to.
 *                If a next memory address is available, the value behind
 *                the address pointer is changed to the next memory address 
 *                to read. Otherwise, the value behind the address pointer
 *                is unchanged.
 * @return true If a next memory address is available, false otherwise.
 *              If false is returned, the value behind the address pointer
 *              is unchanged.
 */
bool address_iterator_next(uint8_t *address);