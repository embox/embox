/**
 * @file
 * 
 * @author Anton Bondarev
 * @date 04.04.2024
 */

#include <util/log.h>

#include <stdint.h>
#include <string.h>

#include <drivers/common/memory.h>

#include <system_k1921vg015.h>

#include <config/board_config.h>

#include <framework/mod/options.h>

#include <drivers/clk/k1921vg015_rcu.h>

#define RCU          ((volatile struct rcu_reg *) CONF_RCU_REGION_BASE)

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

#define RCU_CGCFGAPB_UART_EN(port)  (1 << (6 + port))
#define RCU_CGCFGAPB_UART0EN        (1 << 6)
#define RCU_CGCFGAPB_UART1EN        (1 << 7)
#define RCU_CGCFGAPB_UART2EN        (1 << 8)
#define RCU_CGCFGAPB_UART3EN        (1 << 9)
#define RCU_CGCFGAPB_UART4EN        (1 << 10)

#define RCU_CGCFGAHB_GPIOAEN        (1 << 8)
#define RCU_CGCFGAHB_GPIOBEN        (1 << 9)
#define RCU_CGCFGAHB_GPIOCEN        (1 << 10)

#define RCU_RSTDISAPB_UART_EN(port)  (1 << (6 + port))
#define RCU_RSTDISAPB_UART0EN        (1 << 6)
#define RCU_RSTDISAPB_UART1EN        (1 << 7)
#define RCU_RSTDISAPB_UART2EN        (1 << 8)
#define RCU_RSTDISAPB_UART3EN        (1 << 9)
#define RCU_RSTDISAPB_UART4EN        (1 << 10)

#define RCU_RSTDISAHB_GPIOAEN        (1 << 8)
#define RCU_RSTDISAHB_GPIOBEN        (1 << 9)
#define RCU_RSTDISAHB_GPIOCEN        (1 << 10)

#define RCU_CLKSTAT_SRC_MASK        (0x3)
#define  RCU_CLKSTAT_SRC_HSICLK      (0x0)
#define  RCU_CLKSTAT_SRC_HSECLK      (0x1)
#define  RCU_CLKSTAT_SRC_SYSPLL0CLK  (0x2)
#define  RCU_CLKSTAT_SRC_LSICLK      (0x3)

#define RCU_SYSCLKCFG_SRC_MASK        (0x3)
#define  RCU_SYSCLKCFG_SRC_HSICLK      (0x0)
#define  RCU_SYSCLKCFG_SRC_HSECLK      (0x1)
#define  RCU_SYSCLKCFG_SRC_SYSPLL0CLK  (0x2)
#define  RCU_SYSCLKCFG_SRC_LSICLK      (0x3)

#define RCU_PLLSYSSTAT_LOCK         (0x1)


#define RCU_PLLSYSCFG0_PLLEN        (0x1 << 0)
#define RCU_PLLSYSCFG0_BYP_MASK     (0x3 << 1)
#define RCU_PLLSYSCFG0_DACEN        (0x1 << 3)
#define RCU_PLLSYSCFG0_DSMEN        (0x1 << 4)
#define RCU_PLLSYSCFG0_FOUTEN_MASK  (0x3 << 5)
#define  RCU_PLLSYSCFG0_FOUT0_EN     (0x1 << 5)
#define  RCU_PLLSYSCFG0_FOUT1_EN     (0x2 << 5)
#define RCU_PLLSYSCFG0_REFDIV_MASK  (0x3F << 7) /* 7-12*/
#define RCU_PLLSYSCFG0_REFDIV_VAL(val)   ((val & 0x3F) << 7) /* 7-12*/

#define RCU_UARTCLKCFG0_CLKEN_MASK	 	 	 	0x00000001UL
#define RCU_UARTCLKCFG0_RSTDIS_MASK	 	 	 	0x00000100UL
#define RCU_UARTCLKCFG0_CLKSEL_MASK	 	 	 	0x00030000UL
#define   RCU_UARTCLKCFG0_CLKSEL_SYSPLL0CLK_MASK    0x00020000UL

#define RCU_UARTCLKCFG0_DIVEN_MASK	 	 	 	0x00100000UL
#define RCU_UARTCLKCFG0_DIVN_MASK	 	 	 	0x3f000000UL

int niiet_gpio_clock_setup(unsigned char port) {
	switch (port) {
		case 0:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOAEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOAEN;
		break;
		case 1:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOBEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOBEN;
		break;
		case 2:
			RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOCEN;
			RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOCEN;
		break;		
		default:
			return -1;
	}

	return 0;
}

void niiet_uart_set_rcu(int uart_num) {	
	RCU->RCU_CGCFGAPB_reg |= RCU_CGCFGAPB_UART_EN(uart_num);
	RCU->RCU_RSTDISAPB_reg |= RCU_RSTDISAPB_UART_EN(uart_num);

	RCU->RCU_UARTCLKCFG0_reg = 0;
	RCU->RCU_UARTCLKCFG0_reg |= RCU_UARTCLKCFG0_CLKSEL_SYSPLL0CLK_MASK;

	RCU->RCU_UARTCLKCFG0_reg |= RCU_UARTCLKCFG0_CLKEN_MASK;
    RCU->RCU_UARTCLKCFG0_reg |= RCU_UARTCLKCFG0_RSTDIS_MASK;
}

void niiet_sysclk_init(void) {
    uint32_t sysclk_source;
    
//select system clock
#ifdef CONF_RCU_TYPE_SYSCLK_PLL
//if current system clock is pll that switch to internal
if ((RCU->RCU_CLKSTAT_reg & RCU_CLKSTAT_SRC_MASK) == RCU_CLKSTAT_SRC_SYSPLL0CLK){
    RCU->RCU_SYSCLKCFG_reg = RCU_SYSCLKCFG_SRC_HSICLK;
    // Wait switching done
    while ((RCU->RCU_CLKSTAT_reg & RCU_CLKSTAT_SRC_MASK) != RCU_CLKSTAT_SRC_HSICLK) {
      ;
}
}
//PLLCLK = REFCLK * (FBDIV+FRAC/2^24) / (REFDIV*(1+PD0A)*(1+PD0B))
#if (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 10000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 10 000 000 Hz
    RCU->RCU_PLLSYSCFG1_reg = 0;          //FRAC = 0					 
    RCU->RCU_PLLSYSCFG2_reg = 100;         //FBDIV

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 100;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 4;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 9;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 4;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;
#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 12000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 25 000 000 Hz
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 50;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 3;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 2;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 5;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 3;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;

#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 16000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 12 500 000 Hz
    RCU->RCU_PLLSYSCFG0_reg &= ~(RCU_PLLSYSCFG0_FOUTEN_MASK);
    RCU->RCU_PLLSYSCFG0_reg &= ~(RCU_PLLSYSCFG0_PLLEN);

    RCU->RCU_PLLSYSCFG0_reg = 0; /*clean all fields */

    RCU->RCU_PLLSYSCFG0_reg |= RCU_PLLSYSCFG0_REFDIV_VAL(2);

    RCU->RCU_PLLSYSCFG1_reg = 0;
    RCU->RCU_PLLSYSCFG2_reg = 50;

    RCU->RCU_PLLSYSCFG0_reg |= (3 << 13);/* PLLSYSCFG0_PD0A */
    RCU->RCU_PLLSYSCFG0_reg |= (1 << 16);/* PLLSYSCFG0_PD0B */
    RCU->RCU_PLLSYSCFG0_reg |= (7 << 22);/* PLLSYSCFG0_PD1A */
    RCU->RCU_PLLSYSCFG0_reg |= (7 << 25);/* PLLSYSCFG0_PD1B */

    RCU->RCU_PLLSYSCFG0_reg |= RCU_PLLSYSCFG0_DACEN;

#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 20000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 25 000 000 Hz
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 100;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 4;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 3;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 7;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 4;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;
#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 24000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 30 000 000 Hz
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 65;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 2;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 3;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 7;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 4;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;
#else
#error "Please define HSECLK_VAL with correct values!"
#endif
    RCU->RCU_PLLSYSCFG0_reg |= RCU_PLLSYSCFG0_PLLEN; //.RCU_PLLSYSCFG0_PLLEN = 1; 	// PLL Enable
    RCU->RCU_PLLSYSCFG0_reg |= RCU_PLLSYSCFG0_FOUT0_EN; //.RCU_PLLSYSCFG0_FOUTEN = 1; 	// Fout0 Enable

    //Waiting for PLL to stabilize
    while (!(RCU->RCU_PLLSYSSTAT_reg & RCU_PLLSYSSTAT_LOCK)){
        ;
    };
      
        //select PLL as source system clock
    sysclk_source = RCU_SYSCLKCFG_SRC_SYSPLL0CLK;

#elif defined CONF_RCU_CLK_ENABLE_SYSCLK_HSI
    sysclk_source = RCU_SYSCLKCFG_SRC_REFCLK;
#elif defined CONF_RCU_CLK_ENABLE_SYSCLK_HSE
    sysclk_source = RCU_SYSCLKCFG_SRC_SRCCLK;
#elif defined CONF_RCU_CLK_ENABLE_SYSCLK_LSI
    sysclk_source = RCU_SYSCLKCFG_SRC_LSICLK;
#else
#error "Please define SYSCLK source (SYSCLK_PLL | SYSCLK_HSE | SYSCLK_HSI | SYSCLK_LSI)!"
#endif

    //switch sysclk
    RCU->RCU_SYSCLKCFG_reg |= (sysclk_source << 0);
    // Wait switching done
   while ((RCU->RCU_CLKSTAT_reg & RCU_CLKSTAT_SRC_MASK) != sysclk_source) {
      ;
   }
}
