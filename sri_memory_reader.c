#include <stdio.h>
#include "component.h"
#define BYTE_SIZE 8


int main() {
    printf("Hello World !\n");
    SET_CLK(true);
    unsigned char read_command = 0x55;
    unsigned char address = 0x31;
    unsigned char tail = 0x00;
    int packet = tail << 2*BYTE_SIZE | address << BYTE_SIZE | read_command;

    for (int i = 0; i < 3*BYTE_SIZE; i++) {
        printf("--- Cycle %d ---\n", i);
        SET_CLK(true);

        int mosi_bit = (packet >> i) & 1;
        printf("mosi_bit: %d\n", mosi_bit);
        SET_MOSI(mosi_bit);

        int miso_bit = READ_MISO();
        printf("miso_bit: %d\n", miso_bit);
        SET_CLK(false);
    } 
}