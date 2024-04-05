#ifndef SYSTEM_K1921VG015_H
#define SYSTEM_K1921VG015_H

#include <stdint.h>

#define HSICLK_VAL 1000000
#define LSICLK_VAL 32768

struct gpio_reg {
    uint32_t 	GPIO_DATA_reg;        /* 0x00 */
    uint32_t 	GPIO_DATAOUT_reg;     /* 0x04 */
    uint32_t 	GPIO_DATAOUTSET_reg;  /* 0x08 */
    uint32_t 	GPIO_DATAOUTCLR_reg;  /* 0x0C */
    uint32_t 	GPIO_DATAOUTTGL_reg;  /* 0x10 */
    uint32_t 	GPIO_DENSET_reg;      /* 0x14 */
    uint32_t 	GPIO_DENCLR_reg;      /* 0x18 */
    uint32_t 	GPIO_INMODE_reg;      /* 0x1C */
    uint32_t 	GPIO_PULLMODE_reg;    /* 0x20 */
    uint32_t 	GPIO_OUTMODE_reg;     /* 0x24 */
    uint32_t 	GPIO_DRIVEMODE_reg;   /* 0x28 */
    uint32_t 	GPIO_OUTENSET_reg;    /* 0x2C */
    uint32_t 	GPIO_OUTENCLR_reg;    /* 0x30 */
    uint32_t 	GPIO_ALTFUNCSET_reg;  /* 0x34 */
    uint32_t 	GPIO_ALTFUNCCLR_reg;  /* 0x38 */
    uint32_t 	GPIO_ALTFUNCNUM_reg;  /* 0x3C */
    uint32_t reserved2[1];
    uint32_t 	GPIO_SYNCSET_reg;
    uint32_t 	GPIO_SYNCCLR_reg;
    uint32_t 	GPIO_QUALSET_reg;
    uint32_t 	GPIO_QUALCLR_reg;
    uint32_t 	GPIO_QUALMODESET_reg;
    uint32_t 	GPIO_QUALMODECLR_reg;
    uint32_t 	GPIO_QUALSAMPLE_reg;
    uint32_t 	GPIO_INTENSET_reg;
    uint32_t 	GPIO_INTENCLR_reg;
    uint32_t 	GPIO_INTTYPESET_reg;
    uint32_t 	GPIO_INTTYPECLR_reg;
    uint32_t 	GPIO_INTPOLSET_reg;
    uint32_t 	GPIO_INTPOLCLR_reg;
    uint32_t 	GPIO_INTEDGESET_reg;
    uint32_t 	GPIO_INTEDGECLR_reg;
    uint32_t 	GPIO_INTSTATUS_reg;
    uint32_t 	GPIO_DMAREQSET_reg;
    uint32_t 	GPIO_DMAREQCLR_reg;
    uint32_t 	GPIO_ADCSOCSET_reg;
    uint32_t 	GPIO_ADCSOCCLR_reg;
    uint32_t reserved3[2];
    uint32_t 	GPIO_LOCKKEY_reg;
    uint32_t 	GPIO_LOCKSET_reg;
    uint32_t 	GPIO_LOCKCLR_reg;
};


#define GPIOA_BASE              0x28000000UL
#define GPIOB_BASE              0x28001000UL
#define GPIOC_BASE              0x28002000UL

#define GPIOA                ((volatile struct gpio_reg *) GPIOA_BASE)
#define GPIOB                ((volatile struct gpio_reg *) GPIOB_BASE)
#define GPIOC                ((volatile struct gpio_reg *) GPIOC_BASE)


#define UART0_BASE              0x30006000UL
#define UART1_BASE              0x30007000UL
#define UART2_BASE              0x30008000UL
#define UART3_BASE              0x30009000UL
#define UART4_BASE              0x3000A000UL

#define UART0                ((volatile struct UART_REG *) UART0_BASE)
#define UART1                ((volatile struct UART_REG *) UART1_BASE)
#define UART2                ((volatile struct UART_REG *) UART2_BASE)
#define UART3                ((volatile struct UART_REG *) UART3_BASE)
#define UART4                ((volatile struct UART_REG *) UART4_BASE)


#define TMR32_BASE              0x30000000UL

#define TMR32                ((volatile t_TMR32_REG                *) TMR32_BASE)
#define TMR32_CAPCOM0_VAL   *(( volatile uint32_t          *) 0x3000001CUL)

#define TMR0_BASE              0x30001000UL
#define TMR1_BASE              0x30002000UL
#define TMR2_BASE              0x30003000UL


#define TMR0                ((volatile t_TMR_REG                *) TMR0_BASE)
#define TMR1                ((volatile t_TMR_REG                *) TMR1_BASE)
#define TMR2                ((volatile t_TMR_REG                *) TMR2_BASE)

#define TMR0_CAPCOM0_VAL   *(( volatile uint32_t          *) 0x3000101CUL)

#define RCU_UARTCLKCFG_CLKSEL_PLL0 (0b10)

#endif /* SYSTEM_K1921VG015_H */
