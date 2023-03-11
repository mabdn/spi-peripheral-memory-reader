#include <stdbool.h>
#include <stdint.h>

/*
 * Provides an iterator over the addresses of the memory which the
 * application should read data from.
 *
 * Thus, this method encapsulates the knowledge that the application
 * is supposed to read data from all memory addresses in order from
 * 0x00 to 0xFF in this case.
 *
 * @param address A pointer to a variable which the next memory address
 *                will be written to, if a next memory address is available
 * @return true If a next memory address is available, false otherwise.
 *              If false is returned, the value behind the address pointer
 *              is unchanged.
 */
bool address_iterator_next(uint8_t *address);