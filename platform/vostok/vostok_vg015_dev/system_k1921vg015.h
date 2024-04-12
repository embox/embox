#ifndef SYSTEM_K1921VG015_H
#define SYSTEM_K1921VG015_H

#include <stdint.h>

#define HSICLK_VAL 1000000
#define LSICLK_VAL 32768

#define GPIOA_BASE              0x28000000UL
#define GPIOB_BASE              0x28001000UL
#define GPIOC_BASE              0x28002000UL

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
