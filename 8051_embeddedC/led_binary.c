#include<reg52.h>
#define LED P1
sbit SW=P3^3;
void delay(){
	unsigned int i,j;
	for(i=0;i<500;i++){
	   for(j=0;j<123;j++);
	}
}
void main(){
	unsigned char count =0;
	LED =0x00;
	while(1){
		if(SW ==0){
			count++;
			LED=~count;
			while(SW==0){
			}
		}
	}
}
