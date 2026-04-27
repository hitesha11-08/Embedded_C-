#include <reg52.h>
sbit LED = P1^0;

void delay(){
	unsigned int i,j;
    for(i = 0; i < 500; i++)
        {
					for(	j=0;j<123;j++);
				}
					
}
void main(){
    while(1){
       LED = 0;  
        delay();
        LED = 1;   
        delay();
    }
}
