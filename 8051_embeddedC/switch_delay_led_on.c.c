#include<reg52.h>
sbit LED= P1^0;
sbit SW= P3^3  ;
void delay(){
	int i,j;
	for(i=0;i<200;i++)
	      for(j=0;j<1000;j++);
}
void main(){
	while(1){
				if(SW==0){
					LED =~LED;
					delay();
			}
	else{
		LED=0;
	}
 }
	}