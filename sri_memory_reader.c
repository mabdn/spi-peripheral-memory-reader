#include <stdio.h>
#include "component.h"
#define BYTE_SIZE 8

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

    for (int i = 0; i < 8 * BYTE_SIZE; i++)
    {
        printf("--- Cycle %d ---\n", i);
        SET_CLK(true);

        long mosi_bit = (packet >> i) & 1;
        printf("mosi_bit: %d\n", mosi_bit);
        SET_MOSI(mosi_bit);
        sent_mosi = sent_mosi | (mosi_bit << i);

        long miso_bit = READ_MISO();
        printf("miso_bit: %d\n", miso_bit);
        received_miso = received_miso | (miso_bit << i);

        SET_CLK(false);
    }
    printf("sent_mosi    : %016lx\n", sent_mosi);
    printf("received_miso: %016lx\n", received_miso);
}