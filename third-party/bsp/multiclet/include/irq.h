//Interrupt constant file
#ifndef IRQ_H
#define IRQ_H

#include "R1_ccf.h"

typedef void (*IRQ_HANDLER)(void);

#define IRQ_ENMI 0
#define IRQ_PERE 2
#define IRQ_PRGE 3
#define IRQ_WAKEUP 6 //wake-up
#define IRQ_DTC 7 //DTC
#define IRQ_ST0 8 //ST0
#define IRQ_ST1 9 //ST1
#define IRQ_ST2 10 //ST2
#define IRQ_ST3 11 //ST3
#define IRQ_SW0 12 //SW0
#define IRQ_SW1 13 //SW1
#define IRQ_SW2 14 //SW2
#define IRQ_SW3 15 //SW3
#define IRQ_ADC0 16 //ADC0
#define IRQ_ADC1 18 //ADC1
#define IRQ_UART0 20 //UART0
#define IRQ_UART1 21 //UART1
#define IRQ_UART2 22 //UART2
#define IRQ_UART3 23 //UART3
#define IRQ_I2C0 24 //I2C0
#define IRQ_I2C1 25 //I2C1
#define IRQ_SPI0 26 //SPI0
#define IRQ_SPI1 27 //SPI1
#define IRQ_I2S0 28 //I2S0
#define IRQ_GPTIM0 29 //GPTIM0
#define IRQ_GPTIM1 30 //GPTIM1
#define IRQ_GPTIM2 31 //GPTIM2
#define IRQ_GPTIM3 32 //GPTIM3
#define IRQ_RTC 34 //RTC
#define IRQ_GPIOA 35 //GPIOA
#define IRQ_GPIOB 36 //GPIOB
#define IRQ_GPIOC 37 //GPIOC
#define IRQ_GPIOD 38 //GPIOD
#define IRQ_GPIOE 39 //GPIOE
#define IRQ_GPIOF 40 //GPIOF
#define IRQ_ETHERNET 41 //ETHERNET
#define IRQ_USB 42 //USB
#define IRQ_USB_EPI 43 //USB_EPI
#define IRQ_USB_EPO 44 //USB_EPO
#define IRQ_PWM 45 //PWM
#define IRQ_STAT 46 //STAT

void set_irq_vector(IRQ_HANDLER function, int irq_number);

void asm_setl_MSKR();

int asm_getl_MSKR();
#endif
