#include <stdbool.h>
#include <stdint.h> 
#include "address_iterator.h"
#define START_ADDRESS 0x00
#define END_ADDRESS 0xFF


int address_counter = START_ADDRESS;

bool address_iterator_next(uint8_t *address)
{
    if (address_counter > END_ADDRESS) {
        return false;
    }

    *address = address_counter++;
    return true;
};