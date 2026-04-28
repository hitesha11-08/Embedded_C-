#include <reg52.h>

#define LCD P1

sbit RS = P3^0;
sbit RW = P3^1;
sbit EN = P3^2;

void delay() {
    unsigned int i,j;
    for(i=0;i<500;i++)
        for(j=0;j<123;j++);
}

void lcd_enable() {
    EN = 1;
    delay();
    EN = 0;
    delay();
}

void lcd_cmd(unsigned char cmd) {
    LCD = cmd;
    RS = 0;
    RW = 0;
    lcd_enable();
}

void lcd_data(unsigned char dat) {
    LCD = dat;
    RS = 1;
    RW = 0;
    lcd_enable();
}

void lcd_string(char *str) {
    while(*str) {
        lcd_data(*str++);
    }
}

void lcd_init() {
    lcd_cmd(0x38);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

void first_line(char *str) {
    lcd_cmd(0x80);  
    lcd_string(str);
}

void second_line(char *str) {
    lcd_cmd(0xC0);  
    lcd_string(str);
}

void main() {
    lcd_init();

    first_line("8051");
    second_line("EMBEDDED");

    while(1);
}