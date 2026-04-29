#include<reg52.h>

#define led P1

void delay(){
	int i,j;
	for(i=0;i<200;i++){
		for(j=0;j<200;j++){
		}
	}
}
void fun1(){
	led = 0x00;
	delay();
	led = 0xFF;
	delay();
	led = 0x00;
}

void main(){
	
unsigned char count=0;
fun1();
while(1){
	
led = ~count;
count++;
delay();

 }
}
	
