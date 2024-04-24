#ifndef SYSTEM_K1921VG015_H
#define SYSTEM_K1921VG015_H

#include <stdint.h>

#define HSICLK_VAL 1000000
#define LSICLK_VAL 32768

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
