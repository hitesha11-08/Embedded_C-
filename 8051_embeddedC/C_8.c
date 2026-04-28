#include <reg52.h>

void delay() {
    unsigned int i, j;
    for(i = 0; i < 500; i++)
        for(j = 0; j < 123; j++);
}

void main() {
    unsigned char pattern = 0x80;

    while(1) {
        P1 = pattern;
        delay();
        pattern = pattern >> 1;
        if(pattern == 0x00)
            pattern = 0x80;
    }
}