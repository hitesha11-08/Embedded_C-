#include <stdint.h>

#define RCC_AHB1ENR   (*(volatile uint32_t*)0x40023830)
#define GPIOA_MODER   (*(volatile uint32_t*)0x40020000)
#define GPIOA_ODR     (*(volatile uint32_t*)0x40020014)
#define SYSTICK_LOAD  (*(volatile uint32_t*)0xE000E014)
#define SYSTICK_VAL   (*(volatile uint32_t*)0xE000E018)
#define SYSTICK_CTRL  (*(volatile uint32_t*)0xE000E010)
#define STACK_SIZE    128
/* ───── Semaphore ───── */
typedef struct {
    volatile uint32_t count;   /* 1 = free, 0 = taken */
} semaphore_t;

void sem_init(semaphore_t *s, uint32_t initial)
{
    s->count = initial;
}

void sem_wait(semaphore_t *s)
{
    while (s->count == 0);   /* spin until free  */
    s->count = 0;            /* take it          */
}

void sem_signal(semaphore_t *s)
{
    s->count = 1;            /* release          */
}

semaphore_t gpio_sem;        /* protects GPIOA_ODR */

/* ───── TCB ───── */
typedef struct {
    uint32_t *psp_value;
    uint32_t  delay_count;
} TCB_t;

uint32_t task1_stack[STACK_SIZE];
uint32_t task2_stack[STACK_SIZE];
TCB_t task1_tcb, task2_tcb;
TCB_t *current_tcb;

/* ───── GPIO ───── */
void GPIO_Init(void)
{
    RCC_AHB1ENR |= (1 << 0);
    GPIOA_MODER &= ~(3 << 10);
    GPIOA_MODER |=  (1 << 10);   /* PA5 output */
    GPIOA_MODER &= ~(3 << 12);
    GPIOA_MODER |=  (1 << 12);   /* PA6 output */
}

/* ───── Delay ───── */
void task_delay(volatile TCB_t *tcb, uint32_t ticks)
{
    tcb->delay_count = ticks;
    while (tcb->delay_count != 0);
}

/* ───── Task 1 — PA5 LED, 2-tick period ───── */
void Task1(void)
{
    while (1) {
        sem_wait(&gpio_sem);
        GPIOA_ODR |= (1 << 5);        /* LED ON  */
        sem_signal(&gpio_sem);
        task_delay(&task1_tcb, 2);

        sem_wait(&gpio_sem);
        GPIOA_ODR &= ~(1 << 5);       /* LED OFF */
        sem_signal(&gpio_sem);
        task_delay(&task1_tcb, 2);
    }
}

/* ───── Task 2 — PA6 LED, 4-tick period ───── */
void Task2(void)
{
    while (1) {
        sem_wait(&gpio_sem);
        GPIOA_ODR |= (1 << 6);        /* LED ON  */
        sem_signal(&gpio_sem);
        task_delay(&task2_tcb, 4);

        sem_wait(&gpio_sem);
        GPIOA_ODR &= ~(1 << 6);       /* LED OFF */
        sem_signal(&gpio_sem);
        task_delay(&task2_tcb, 4);
    }
}

/* ───── Stack init ───── */
void Init_task_stack(TCB_t *tcb, uint32_t *stack, void (*task)(void))
{
    uint32_t *psp = stack + STACK_SIZE;
    *(--psp) = 0x01000000;   /* xPSR  */
    *(--psp) = (uint32_t)task;
    *(--psp) = 0xFFFFFFFD;   /* LR    */
    *(--psp) = 0;            /* R12   */
    *(--psp) = 0;            /* R3    */
    *(--psp) = 0;            /* R2    */
    *(--psp) = 0;            /* R1    */
    *(--psp) = 0;            /* R0    */
    *(--psp) = 0; *(--psp) = 0; *(--psp) = 0; *(--psp) = 0;
    *(--psp) = 0; *(--psp) = 0; *(--psp) = 0; *(--psp) = 0;
    tcb->psp_value  = psp;
    tcb->delay_count = 0;
}

/* ───── PSP helpers ───── */
void set_psp(uint32_t v)
{
    __asm volatile ("MSR PSP,%0" : : "r"(v));
}

void Switch_to_psp(void)
{
    __asm volatile (
        "MOV R0,#0x02  \n"
        "MSR CONTROL,R0\n"
        "ISB           \n"
    );
}

/* ───── SysTick ───── */
void SysTick_Init(void)
{
    SYSTICK_CTRL = 0;
    SYSTICK_LOAD = 16000000 - 1;
    SYSTICK_VAL  = 0;
    SYSTICK_CTRL = 0x07;
}

/* ───── Scheduler ───── */
void schedule(void)
{
    if (task1_tcb.delay_count) task1_tcb.delay_count--;
    if (task2_tcb.delay_count) task2_tcb.delay_count--;

    if (current_tcb == &task1_tcb) {
        if      (task2_tcb.delay_count == 0) current_tcb = &task2_tcb;
        else if (task1_tcb.delay_count == 0) current_tcb = &task1_tcb;
    } else {
        if      (task1_tcb.delay_count == 0) current_tcb = &task1_tcb;
        else if (task2_tcb.delay_count == 0) current_tcb = &task2_tcb;
    }
}

/* ───── SysTick handler (context switch) ───── */
__attribute__((naked)) void SysTick_Handler(void)
{
    __asm volatile (
        "PUSH {LR}            \n"
        "MRS  R0, PSP         \n"
        "STMDB R0!, {R4-R11}  \n"
        "LDR  R1, =current_tcb\n"
        "LDR  R2, [R1]        \n"
        "STR  R0, [R2]        \n"
        "BL   schedule        \n"
        "LDR  R1, =current_tcb\n"
        "LDR  R2, [R1]        \n"
        "LDR  R0, [R2]        \n"
        "LDMIA R0!, {R4-R11}  \n"
        "MSR  PSP, R0         \n"
        "POP  {LR}            \n"
        "BX   LR              \n"
    );
}

/* ───── Main ───── */
int main(void)
{
    GPIO_Init();
    sem_init(&gpio_sem, 1);          /* semaphore starts free */
    Init_task_stack(&task1_tcb, task1_stack, Task1);
    Init_task_stack(&task2_tcb, task2_stack, Task2);
    current_tcb = &task1_tcb;
    set_psp((uint32_t)task1_tcb.psp_value);
    Switch_to_psp();
    SysTick_Init();
    Task1();
    while (1);
}
