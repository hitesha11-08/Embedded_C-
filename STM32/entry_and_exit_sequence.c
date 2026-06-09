#include <stdint.h>

#define NVIC_ISER0 (*(volatile uint32_t*)0xE000E100) /*memmap for interrupt service enable reg*/
#define NVIC_ISPR0 (*(volatile uint32_t*)0xE000E200) /*memmap for interrupt service pending reg*/
#define NVIC_IABR0 (*(volatile uint32_t*)0xE000E300) /*memmap for interrupt active bit reg*/

volatile uint32_t ipsr_before;
volatile uint32_t ipsr_in_isr;                /*variables for pending bit*/
volatile uint32_t ipsr_after;

volatile uint32_t active_before;
volatile uint32_t active_in_isr;              /*variables for active bit*/
volatile uint32_t active_after;

volatile uint32_t exc_return;                   /*exc_return */

static inline uint32_t Read_IPSR(void)
{
    uint32_t result;

    __asm volatile(
        "MRS %0, IPSR"                          /*inline or assembly code*/
        : "=r"(result)
    );

    return result;
}

void EXTI0_IRQHandler(void)              /*exception ocurrs*/
{
                                        /*entry process */

    ipsr_in_isr = Read_IPSR();

    active_in_isr = NVIC_IABR0;

    __asm volatile(                   /*special exc_return value stored in reg LR*/
        "MOV %0, LR"
        : "=r"(exc_return)
    );

    for(volatile int i=0;i<1000;i++);

                                           /* exxit occurs when BX LR is executed*/
}

int main(void)
{
                                               /* thread mode */

    ipsr_before = Read_IPSR();

    active_before = NVIC_IABR0;

                                                   /* ENABLE IRQ6 (EXTI0) */

    NVIC_ISER0 |= (1<<6);

                                                   /* SET PENDING BIT */

    NVIC_ISPR0 |= (1<<6);

    for(volatile int i=0;i<100000;i++);

                                                    /* BACK TO THREAD MODE */

    ipsr_after = Read_IPSR();

    active_after = NVIC_IABR0;

    while(1);
}
