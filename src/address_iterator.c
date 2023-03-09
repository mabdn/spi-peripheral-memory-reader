#include <stdbool.h> 
#include "address_iterator.h"
#define START_ADDRESS 0x00
#define END_ADDRESS 0xFF


int address_counter = START_ADDRESS;

bool address_iterator_next(unsigned char *address)
{
    if (address_counter > END_ADDRESS) {
        return false;
    }

    *address = address_counter++;
    return true;
};