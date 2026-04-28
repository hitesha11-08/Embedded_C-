#include<reg52.h>
sbit LED1 = P2^0;
sbit LED2 = P2^1;
sbit LED3 = P2^2;

void delay(){
	unsigned int i,j;
	    for(i=0;i<500;i++){
				for(j=0;j<123;j++);
				}
			}
void main(){
while(1){
  LED1=0;
  LED2=1;
  LED3=1;
	delay(); 
	LED1=1;
  LED2=0;
  LED3=1;
	delay(); 
	LED1=1;
  LED2=1;
  LED3=0;
	delay(); 
}
}
	

	
