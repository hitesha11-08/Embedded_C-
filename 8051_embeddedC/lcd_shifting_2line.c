#include <reg52.h>

#define LCD P2

sbit RS = P3^0;
sbit RW = P3^1;
sbit EN = P3^2;
void delay() {
    int i,j;
    for(i=0;i<100;i++)
        for(j=0;j<1275;j++);
}
void send_cmd(unsigned char c) {
    LCD = c;
    RS = 0;
    RW = 0;
    EN = 1;
    delay();
    EN = 0;
}
void send_data(unsigned char d) {
    LCD = d;
    RS = 1;
    RW = 0;
    EN = 1;
    delay();
    EN = 0;
}
void lcd_init() {
    send_cmd(0x38);
    send_cmd(0x0C);
    send_cmd(0x01);
}
void line1() {
    send_cmd(0x80);
}

void line2() {
    send_cmd(0xC0);
}

// Print string
void print(char *p) {
    while(*p)
        send_data(*p++);
}
void clear() {
    send_cmd(0x01);
}

void main() {

    lcd_init();

    while(1) {

        line1();
        print("HITESHA M");

        line2();
        print("EMBEDDED ENG");

        delay();

        clear();
        delay();
    }
}
