#include <stdint.h>
#define RCC_AHB1ENR (*(volatile uint32_t*)0x40023830)
#define GPIOA_MODER (*(volatile uint32_t*)0x40020000)
#define GPIOA_ODR (*(volatile uint32_t*)0x40020014)
void delay()
{
for(int i=0;i<50000;i++);
}
int main(void)
{
RCC_AHB1ENR|=(1<<0);
GPIOA_MODER&=~(3<<10);
GPIOA_MODER|=(1<<10);
while(1)
{
__asm volatile(
"LDR R1,[%0]\n"
"ORR R1,R1,#(1<<5)\n"
"STR R1,[%0]\n"
:
:"r"(&GPIOA_ODR)
:"r1"
);

delay();

__asm volatile(
"LDR R1,[%0]\n"
"BIC R1,R1,#(1<<5)\n"
"STR R1,[%0]\n"
:
:"r"(&GPIOA_ODR)
:"r1"
);

delay();
}
}
