#include <stdint.h>

/* RCC */
#define RCC_AHB1ENR   (*(volatile uint32_t*)0x40023830)
#define RCC_APB1ENR   (*(volatile uint32_t*)0x40023840)

/* GPIOB */
#define GPIOB_MODER   (*(volatile uint32_t*)0x40020400)
#define GPIOB_OTYPER  (*(volatile uint32_t*)0x40020404)
#define GPIOB_PUPDR   (*(volatile uint32_t*)0x4002040C)
#define GPIOB_AFRH    (*(volatile uint32_t*)0x40020424)

/* GPIOA USART2 */
#define GPIOA_MODER   (*(volatile uint32_t*)0x40020000)
#define GPIOA_AFRL    (*(volatile uint32_t*)0x40020020)

/* USART2 */
#define USART2_SR     (*(volatile uint32_t*)0x40004400)
#define USART2_DR     (*(volatile uint32_t*)0x40004404)
#define USART2_BRR    (*(volatile uint32_t*)0x40004408)
#define USART2_CR1    (*(volatile uint32_t*)0x4000440C)

/* I2C1 */
#define I2C1_CR1      (*(volatile uint32_t*)0x40005400)
#define I2C1_CR2      (*(volatile uint32_t*)0x40005404)
#define I2C1_DR       (*(volatile uint32_t*)0x40005410)
#define I2C1_SR1      (*(volatile uint32_t*)0x40005414)
#define I2C1_SR2      (*(volatile uint32_t*)0x40005418)
#define I2C1_CCR      (*(volatile uint32_t*)0x4000541C)
#define I2C1_TRISE    (*(volatile uint32_t*)0x40005420)

#define MAX30102_ADDR 0x57

void delay(void)
{
    for(volatile int i=0;i<50000;i++);
}

/* USART2 INIT */
void USART2_Init(void)
{
    RCC_AHB1ENR |= (1<<0);
    RCC_APB1ENR |= (1<<17);

    /* PA2 TX */
    GPIOA_MODER &= ~(3U<<4);
    GPIOA_MODER |=  (2U<<4);

    GPIOA_AFRL &= ~(0xF<<8);
    GPIOA_AFRL |=  (7<<8);

    USART2_BRR = 0x0683;

    USART2_CR1 |= (1<<13);
    USART2_CR1 |= (1<<3);
}

void USART2_SendChar(char c)
{
    while(!(USART2_SR & (1<<7)));

    USART2_DR = c;
}

void USART2_SendString(char *str)
{
    while(*str)
    {
        USART2_SendChar(*str++);
    }
}

void USART2_SendNumber(uint32_t num)
{
    char buf[10];
    int i=0;

    if(num==0)
    {
        USART2_SendChar('0');
        return;
    }

    while(num>0)
    {
        buf[i++] = (num%10)+'0';
        num/=10;
    }

    while(i>0)
    {
        USART2_SendChar(buf[--i]);
    }
}

/* GPIO I2C */
void GPIO_I2C_Init(void)
{
    RCC_AHB1ENR |= (1<<1);

    GPIOB_MODER &= ~((3U<<16)|(3U<<18));
    GPIOB_MODER |=  ((2U<<16)|(2U<<18));

    GPIOB_OTYPER |= (1<<8)|(1<<9);

    GPIOB_PUPDR &= ~((3U<<16)|(3U<<18));
    GPIOB_PUPDR |=  ((1U<<16)|(1U<<18));

    GPIOB_AFRH &= ~((0xF<<0)|(0xF<<4));
    GPIOB_AFRH |=  ((4<<0)|(4<<4));
}

/* I2C INIT */
void I2C1_Init(void)
{
    RCC_APB1ENR |= (1<<21);

    I2C1_CR1 |= (1<<15);
    I2C1_CR1 &= ~(1<<15);

    I2C1_CR2 = 16;

    I2C1_CCR = 80;

    I2C1_TRISE = 17;

    I2C1_CR1 |= (1<<0);
}

void I2C1_Start(void)
{
    I2C1_CR1 |= (1<<8);

    while(!(I2C1_SR1 & (1<<0)));
}

void I2C1_Stop(void)
{
    I2C1_CR1 |= (1<<9);
}

void I2C1_Address(uint8_t addr)
{
    I2C1_DR = addr;

    while(!(I2C1_SR1 & (1<<1)));

    (void)I2C1_SR1;
    (void)I2C1_SR2;
}

void I2C1_Write(uint8_t data)
{
    while(!(I2C1_SR1 & (1<<7)));

    I2C1_DR = data;

    while(!(I2C1_SR1 & (1<<2)));
}

uint8_t I2C1_Read_Ack(void)
{
    I2C1_CR1 |= (1<<10);

    while(!(I2C1_SR1 & (1<<6)));

    return I2C1_DR;
}

uint8_t I2C1_Read_Nack(void)
{
    I2C1_CR1 &= ~(1<<10);

    while(!(I2C1_SR1 & (1<<6)));

    return I2C1_DR;
}

/* MAX30102 WRITE */
void MAX30102_Write(uint8_t reg,uint8_t value)
{
    I2C1_Start();

    I2C1_Address(MAX30102_ADDR<<1);

    I2C1_Write(reg);

    I2C1_Write(value);

    I2C1_Stop();
}

/* MAX30102 READ */
uint8_t MAX30102_Read(uint8_t reg)
{
    uint8_t data;

    I2C1_Start();

    I2C1_Address(MAX30102_ADDR<<1);

    I2C1_Write(reg);

    I2C1_Start();

    I2C1_Address((MAX30102_ADDR<<1)|1);

    data = I2C1_Read_Nack();

    I2C1_Stop();

    return data;
}

/* SENSOR INIT */
void MAX30102_Init(void)
{
    MAX30102_Write(0x09,0x40);
    delay();

    MAX30102_Write(0x09,0x03);

    MAX30102_Write(0x0A,0x27);

    MAX30102_Write(0x0C,0xFF);

    MAX30102_Write(0x0D,0xFF);
}

int main(void)
{
    uint32_t red_data;

    GPIO_I2C_Init();

    I2C1_Init();

    USART2_Init();

    MAX30102_Init();

    while(1)
    {
        /* Read RED FIFO */
        red_data  = ((uint32_t)MAX30102_Read(0x07)<<16);
        red_data |= ((uint32_t)MAX30102_Read(0x08)<<8);
        red_data |= ((uint32_t)MAX30102_Read(0x09));

        red_data &= 0x03FFFF;

        USART2_SendString("RED=");
        USART2_SendNumber(red_data);
        USART2_SendString("\r\n");

        delay();
    }
}
