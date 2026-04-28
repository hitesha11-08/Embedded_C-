#include <reg52.h>
#define LCD P1
sbit RS = P3^0;
sbit RW = P3^1;
sbit EN = P3^2;
sbit SW  = P3^3;
sbit LED = P2^0;
void delay(){
    unsigned int i,j;
    for(i=0;i<500;i++)
        for(j=0;j<123;j++);
}
void cmd(unsigned char c){
    LCD = c;
    RS = 0;
    RW = 0;
    EN = 1;
    delay();
    EN = 0;
}
void lcd_data(unsigned char d){
    LCD = d;
    RS = 1;
    RW = 0;
    EN = 1;
    delay();
    EN = 0;
}

void str(char *s){
    while(*s){
        lcd_data(*s++);
    }
}

void init(){
    cmd(0x38);
    cmd(0x0C);
    cmd(0x06);
    cmd(0x01);
}
void main(){
    init();

    while(1){
        cmd(0x80);

        if(SW == 0){
            LED = 0;                 
            str("LED ON");
        }
        else{
            LED = 1;             
            str("LED OFF");
        }

        delay();
        cmd(0x01);
    }
}
