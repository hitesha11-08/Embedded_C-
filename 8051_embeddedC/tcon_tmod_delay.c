#include <reg52.h>

sbit LED = P2^0;

void delay() {
    TMOD = 0x01;
    TH0 = 0xFF;
    TL0 = 0x00	;

TR0 = 1;    
    while(TF0 == 0);
    TR0 = 0;
    TF0 = 0;
}
void main() {
    while(1) {
        LED = 0;
        delay();

        LED = 1;
        delay();
    }
}