#ifdef SYSTEM_K1921VG015_H
#define SYSTEM_K1921VG015_H

#include <stdint.h>

#define HSICLK_VAL 1000000
#define LSICLK_VAL 32768

struct rcu_reg { 
    uint32_t 	RCU_CGCFGAHB_reg;   /* 0x000 */
    uint32_t reserved0[1];
    uint32_t 	RCU_CGCFGAPB_reg;   /* 0x008 */
    uint32_t reserved1[1];
    uint32_t 	RCU_RSTDISAHB_reg;  /* 0x010 */
    uint32_t reserved2[1];
    uint32_t 	RCU_RSTDISAPB_reg;  /* 0x018 */
    uint32_t reserved3[1];
    uint32_t 	RCU_RSTSTAT_reg;    /* 0x020 */
    uint32_t reserved4[3];
    uint32_t 	RCU_SYSCLKCFG_reg;  /* 0x030 */
    uint32_t 	RCU_SECCNT0_reg;    /* 0x034 */
    uint32_t 	RCU_SECCNT1_reg;    /* 0x038 */
    uint32_t 	RCU_CLKSTAT_reg;    /* 0x03C */
    uint32_t reserved5[4];
    uint32_t 	RCU_PLLSYSCFG0_reg; /* 0x050 */
    uint32_t 	RCU_PLLSYSCFG1_reg; /* 0x054 */
    uint32_t 	RCU_PLLSYSCFG2_reg; /* 0x058 */
    uint32_t 	RCU_PLLSYSCFG3_reg; /* not in doc 0x05C */
    uint32_t 	RCU_PLLSYSSTAT_reg; /* 0x060 */
    uint32_t reserved6[3];
    uint32_t 	RCU_UARTCLKCFG0_reg; /* 0x070 */
    uint32_t 	RCU_UARTCLKCFG1_reg; /* 0x074 */
    uint32_t 	RCU_UARTCLKCFG2_reg; /* 0x078 */
    uint32_t 	RCU_UARTCLKCFG3_reg; /* 0x07C */
    uint32_t 	RCU_UARTCLKCFG4_reg; /* 0x080 */    
    uint32_t reserved7[3];
    uint32_t 	RCU_QSPICLKCFG_reg; /* 0x090 */
    uint32_t 	RCU_SPICLKCFG0_reg; /* 0x094 */
    uint32_t 	RCU_SPICLKCFG1_reg; /* 0x098 */
    uint32_t reserved8[5];
    uint32_t 	RCU_ADCSARCLKCFG_reg; /* 0x0B0 */
    uint32_t 	RCU_ADCSDCLKCFG_reg; /* 0x0B4 */
    uint32_t 	RCU_WDOGCLKCFG_reg;  /* 0x0B8 */
    uint32_t 	RCU_CLKOUTCFG_reg;   /* 0x0BC */
    uint32_t 	RCU_RSTSYS_reg;      /* 0x0C0 */
} ;


struct GPIO_REG {
    uint32_t 	GPIO_DATA_reg;
    uint32_t 	GPIO_DATAOUT_reg;
    uint32_t 	GPIO_DATAOUTSET_reg;
    uint32_t 	GPIO_DATAOUTCLR_reg;
    uint32_t 	GPIO_DATAOUTTGL_reg;
    uint32_t reserved0[2];
    uint32_t 	GPIO_INMODE_reg;
    uint32_t 	GPIO_PULLMODE_reg;
    uint32_t 	GPIO_OUTMODE_reg;
    uint32_t reserved1[1];
    uint32_t 	GPIO_OUTENSET_reg;
    uint32_t 	GPIO_OUTENCLR_reg;
    uint32_t 	GPIO_ALTFUNCSET_reg;
    uint32_t 	GPIO_ALTFUNCCLR_reg;
    uint32_t 	GPIO_ALTFUNCNUM_reg;
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

#define GPIOA                ((volatile struct GPIO_REG *) GPIOA_BASE)
#define GPIOB                ((volatile struct GPIO_REG *) GPIOB_BASE)
#define GPIOC                ((volatile struct GPIO_REG *) GPIOC_BASE)


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

#define RCU_BASE      (0x3000E000UL)
#define RCU          ((volatile struct RCU_REG *) RCU_BASE)

#define RCU_UARTCLKCFG_CLKSEL_PLL0 (0b10)

#endif /* SYSTEM_K1921VG015_H */