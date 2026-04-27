#include <reg52.h>

void delay() {
    unsigned int i, j;
    for(i = 0; i < 500; i++)
        for(j = 0; j < 123; j++);
}

void main() {
    while(1) {
        P1 = 0x55;
        delay();
        P1 = 0xAA;
        delay();
    }
}