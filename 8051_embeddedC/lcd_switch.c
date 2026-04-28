#include <reg52.h>
#define LCD P1
sbit RS = P3^0;
sbit RW = P3^1;
sbit EN = P3^2;
sbit SW = P3^3;

void delay(){
    unsigned int i,j;
    for(i=0;i<500;i++)
        for(j=0;j<123;j++);
}

void lcd_cmd(unsigned char cmd){
    LCD = cmd;
    RS = 0;
    RW = 0;
    EN = 1;
    delay();
    EN = 0;
}

void lcd_data(unsigned char dat){
    LCD = dat;
    RS = 1;
    RW = 0;
    EN = 1;
    delay();
    EN = 0;
}

void lcd_string(char *str){
    while(*str){
        lcd_data(*str++);
    }
}

void lcd_init(){
    lcd_cmd(0x38);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

void main(){
    lcd_init();

    while(1){
        lcd_cmd(0x80);

        if(SW == 0)
            lcd_string("PRESSED");
        else
            lcd_string("NOT PRESSED");

        delay();
        lcd_cmd(0x01);
    }
}
