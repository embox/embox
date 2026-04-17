/**
 * @file
 * @brief RCU (Reset and Clock Unit) driver for NIIET K1921VG1T
 *
 * @author Anton Bondarev
 * @date 05.03.2026
 */

#include <util/log.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <drivers/common/memory.h>

#include <config/board_config.h>

#include <framework/mod/options.h>

#include <drivers/clk/niiet_rcu.h>

/* FROM board_config.h*/
#define CLK_NAME_GPIO     "CLK_GPIO"
#define CLK_NAME_UART     "CLK_UART"
#define CLK_NAME_SPI      "CLK_SPI"
#define CLK_NAME_I2C      "CLK_I2C"
#define CLK_NAME_TMR      "CLK_TMR"

#define RCU          ((volatile struct rcu_reg *) CONF_RCU_REGION_BASE)

struct rcu_syscfg_reg {
    uint32_t CFG;
    uint32_t DIV;
    uint32_t FRAC;
    uint32_t MOD;
};

struct rcu_reg { 
    uint32_t 	RCU_CGCFGAHB_reg;   /* 0x000 */
    uint32_t    RCU_CGCFGAPB0_reg;
    uint32_t 	RCU_CGCFGAPB1_reg;   /* 0x008 */
    uint32_t    RCU_CGCFGAPB2_reg;
    uint32_t 	RCU_RSTDISAHB_reg;  /* 0x010 */
    uint32_t    RCU_RSTDISAPB0_reg;
    uint32_t 	RCU_RSTDISAPB1_reg;  /* 0x018 */
    uint32_t    RCU_RSTDISAPB2_reg;
    uint32_t 	RCU_SYSCLKCFG_reg;  /* 0x020 */
    uint32_t 	RCU_SECCNT0_reg;    /* 0x024 */
    uint32_t 	RCU_SECCNT1_reg;    /* 0x028 */
    uint32_t 	RCU_CLKSTAT_reg;    /* 0x02C */
    uint32_t 	RCU_RSTSYS_reg;    /* 0x030 */
    uint32_t 	RCU_RSTSTAT_reg;    /* 0x034 */
    uint32_t 	RCU_BORCFG_reg;    /* 0x038 */
    uint32_t 	RCU_LOCK_reg;    /* 0x03C */
  
    uint32_t 	RCU_INTEN_reg;    /* 0x040 */
    uint32_t 	RCU_INTSTAT_reg;    /* 0x044 */
    uint32_t 	RCU_HSECFG_reg;    /* 0x048 */
    uint32_t reserved0_reg[1];    /* 0x04C */

    uint32_t 	RCU_UARTCLKCFG_reg[8];
    uint32_t 	RCU_SPICLKCFG_reg[8];

    uint32_t 	RCU_ADCCFG_reg;
    uint32_t 	RCU_WDTCFG_reg;
    uint32_t 	RCU_CLKOUTCFG_reg;
    uint32_t 	RCU_SDRAMCFG_reg;

    uint32_t    RCU_I2SCFG_reg[2];
    uint32_t reserved1_reg[2];
    uint32_t 	RCU_USBCFG_reg[2];
    uint32_t 	RCU_ETHCFG_reg;
    uint32_t 	RCU_CANFDCFG_reg;
    uint32_t 	RCU_FLASHCFG_reg;
    uint32_t 	RCU_RTCCFG_reg;

    uint32_t reserved3[14];

    struct rcu_syscfg_reg RCU_PLLCFG_reg[3];

    uint32_t 	RCU_PLLSTAT_reg;
};

#define RCU_CGCFGAPB_TMR_OFFSET     16
#define RCU_CGCFGAPB_TMR0EN         (1 << (RCU_CGCFGAPB_TMR_OFFSET + 0))
#define RCU_CGCFGAPB_TMR1EN         (1 << (RCU_CGCFGAPB_TMR_OFFSET + 1))
#define RCU_CGCFGAPB_TMR2EN         (1 << (RCU_CGCFGAPB_TMR_OFFSET + 2))
#define RCU_CGCFGAPB_TMR_EN(nr)     (1 << (RCU_CGCFGAPB_TMR_OFFSET + nr))

#define RCU_CGCFGAPB_UART_EN(port)  (1 << (16 + port))


#define RCU_CGCFGAPB_I2C_EN(port)   (1 << (5 + port))
#define RCU_CGCFGAPB_I2C0EN         (1 << 5)

#define RCU_CGCFGAHB_GPIOEN(port)   (1 << (0 + port))


#define RCU_CGCFGAHB_SPI0EN         (1 << 5)
#define RCU_CGCFGAHB_SPI1EN         (1 << 6)
#define RCU_CGCFGAHB_SPI_EN(nr)     (1 << (5 + nr))

#define RCU_RSTDISAPB_TMR_OFFSET     16
#define RCU_RSTDISAPB_TMR0EN         (1 << (RCU_RSTDISAPB_TMR_OFFSET + 0))
#define RCU_RSTDISAPB_TMR1EN         (1 << (RCU_RSTDISAPB_TMR_OFFSET + 1))
#define RCU_RSTDISAPB_TMR2EN         (1 << (RCU_RSTDISAPB_TMR_OFFSET + 2))
#define RCU_RSTDISAPB_TMR_EN(num)    (1 << (RCU_RSTDISAPB_TMR_OFFSET + num))

#define RCU_RSTDISAPB_UART_EN(port)  (1 << (16 + port))
#define RCU_RSTDISAPB_UART0EN        (1 << 6)
#define RCU_RSTDISAPB_UART1EN        (1 << 7)
#define RCU_RSTDISAPB_UART2EN        (1 << 8)
#define RCU_RSTDISAPB_UART3EN        (1 << 9)
#define RCU_RSTDISAPB_UART4EN        (1 << 10)

#define RCU_RSTDISAPB_I2C_EN(port)   (1 << (5 + port))
#define RCU_RSTDISAPB_I2C0EN         (1 << 5)

#define RCU_RSTDISAHB_GPIO_OFFSET    0
#define RCU_RSTDISAHB_GPIOEN(port)   (1 << (RCU_RSTDISAHB_GPIO_OFFSET + port))
#define RCU_RSTDISAHB_GPIOAEN        (1 << (RCU_RSTDISAHB_GPIO_OFFSET + 0))
#define RCU_RSTDISAHB_GPIOBEN        (1 << (RCU_RSTDISAHB_GPIO_OFFSET + 1))
#define RCU_RSTDISAHB_GPIOCEN        (1 << (RCU_RSTDISAHB_GPIO_OFFSET + 2))

#define RCU_RSTDISAHB_SPI0EN         (1 << 5)
#define RCU_RSTDISAHB_SPI1EN         (1 << 6)
#define RCU_RSTDISAHB_SPI_EN(num)    (1 << (5 + num))

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
#define RCU_SYSCLKCFG_SRC_Pos         (0x0)

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

/* Bit field positions: */
#define RCU_PLL_CFG_PD_Pos                    0                      /*!< Powerdown enable */
#define RCU_PLL_CFG_BYPASS_Pos                1                      /*!< Bypass enable */
#define RCU_PLL_CFG_FOUTEN_Pos                2                      /*!< Fout enable */
#define RCU_PLL_CFG_CLKSEL_Pos                3                      /*!< Select reference clock */
#define RCU_PLL_CFG_SP_Pos                    4                      /*!< Song-Park mode */
#define RCU_PLL_CFG_ST_Pos                    5                      /*!< Sigma-delta modulator mode */
#define RCU_PLL_CFG_CP_Pos                    8                      /*!< Output current of current switch */
#define RCU_PLL_CFG_PFD_Pos                   12                     /*!< Mode of current switch */
#define RCU_PLL_CFG_LPF_Pos                   14                     /*!< Loop filter band */
#define RCU_PLL_CFG_VCOMODE_Pos               16                     /*!< Subband selection signal */
#define RCU_PLL_CFG_VCOMA_Pos                 20                     /*!< Ajust amplitude of GUN */
#define RCU_PLL_CFG_VCOMF_Pos                 21                     /*!< Ajust lower range of GUN */
#define RCU_PLL_CFG_DLDMODE_Pos               22                     /*!< Frequency detector delay */
#define RCU_PLL_CFG_DT_Pos                    24                     /*!< Fractional noise randomization mode */
#define RCU_PLL_CFG_MXEN_Pos                  28                     /*!< Test mux enable */
#define RCU_PLL_CFG_MX_Pos                    29                     /*!< Test mux signal select */

/* Bit field masks: */
#define RCU_PLL_CFG_PD_Msk                    0x00000001UL           /*!< Powerdown enable */
#define RCU_PLL_CFG_BYPASS_Msk                0x00000002UL           /*!< Bypass enable */
#define RCU_PLL_CFG_FOUTEN_Msk                0x00000004UL           /*!< Fout enable */
#define RCU_PLL_CFG_CLKSEL_Msk                0x00000008UL           /*!< Select reference clock */
#define RCU_PLL_CFG_SP_Msk                    0x00000010UL           /*!< Song-Park mode */
#define RCU_PLL_CFG_ST_Msk                    0x000000E0UL           /*!< Sigma-delta modulator mode */
#define RCU_PLL_CFG_CP_Msk                    0x00000F00UL           /*!< Output current of current switch */
#define RCU_PLL_CFG_PFD_Msk                   0x00003000UL           /*!< Mode of current switch */
#define RCU_PLL_CFG_LPF_Msk                   0x0000C000UL           /*!< Loop filter band */
#define RCU_PLL_CFG_VCOMODE_Msk               0x000F0000UL           /*!< Subband selection signal */
#define RCU_PLL_CFG_VCOMA_Msk                 0x00100000UL           /*!< Ajust amplitude of GUN */
#define RCU_PLL_CFG_VCOMF_Msk                 0x00200000UL           /*!< Ajust lower range of GUN */
#define RCU_PLL_CFG_DLDMODE_Msk               0x00400000UL           /*!< Frequency detector delay */
#define RCU_PLL_CFG_DT_Msk                    0x03000000UL           /*!< Fractional noise randomization mode */
#define RCU_PLL_CFG_MXEN_Msk                  0x10000000UL           /*!< Test mux enable */
#define RCU_PLL_CFG_MX_Msk                    0xE0000000UL           /*!< Test mux signal select */

/* Bit field positions: */
#define RCU_PLL_DIV_DIV1A_Pos                 0                      /*!< Divider 1A */
#define RCU_PLL_DIV_DIV1B_Pos                 4                      /*!< Divider 1B */
#define RCU_PLL_DIV_PREDIV_Pos                12                     /*!< Predivisor coefficient */
#define RCU_PLL_DIV_NNCLR_Pos                 14                     /*!< N-divider enable */
#define RCU_PLL_DIV_RNCLR_Pos                 15                     /*!< R-divider enable */
#define RCU_PLL_DIV_RDIV_Pos                  16                     /*!< R-divider coefficient */
#define RCU_PLL_DIV_NDIV_Pos                  24                     /*!< Integer part of divider */

/* Bit field masks: */
#define RCU_PLL_DIV_DIV1A_Msk                 0x00000007UL           /*!< Divider 1A */
#define RCU_PLL_DIV_DIV1B_Msk                 0x000003F0UL           /*!< Divider 1B */
#define RCU_PLL_DIV_PREDIV_Msk                0x00003000UL           /*!< Predivisor coefficient */
#define RCU_PLL_DIV_NNCLR_Msk                 0x00004000UL           /*!< N-divider enable */
#define RCU_PLL_DIV_RNCLR_Msk                 0x00008000UL           /*!< R-divider enable */
#define RCU_PLL_DIV_RDIV_Msk                  0x003F0000UL           /*!< R-divider coefficient */
#define RCU_PLL_DIV_NDIV_Msk                  0xFF000000UL           /*!< Integer part of divider */

/* Bit field positions: */
#define RCU_PLL_FRAC_FRAC_Pos                 0                      /*!< Numerator of fractional part */

/* Bit field masks: */
#define RCU_PLL_FRAC_FRAC_Msk                 0x00FFFFFFUL           /*!< Numerator of fractional part */

/* Bit field positions: */
#define RCU_PLL_MOD_MOD_Pos                   0                      /*!< Denominator of fractional part */

/* Bit field masks: */
#define RCU_PLL_MOD_MOD_Msk                   0x00FFFFFFUL           /*!< Denominator of fractional part */

#define RCU_UARTCLKCFG_CLKEN_MASK               0x00000001UL
#define RCU_UARTCLKCFG_RSTDIS_MASK              0x00000010UL
#define RCU_UARTCLKCFG_CLKSEL_OFFSET             (8)
#define RCU_UARTCLKCFG_CLKSEL_MASK               0x00000300UL
# define  RCU_UARTCLKCFG_CLKSEL_HSICLK_MASK      0x00000000UL
# define  RCU_UARTCLKCFG_CLKSEL_HSECLK_MASK      0x00000100UL
# define  RCU_UARTCLKCFG_CLKSEL_SYSPLL0CLK_MASK  0x00000200UL
# define  RCU_UARTCLKCFG_CLKSEL_SYSPLL1CLK_MASK  0x00000300UL
#define RCU_UARTCLKCFG_DIVEN_MASK               0x00001000UL
#define RCU_UARTCLKCFG_DIVN_MASK                0x003F0000UL


int niiet_gpio_clock_setup(unsigned char port) {
    RCU->RCU_CGCFGAHB_reg |= RCU_CGCFGAHB_GPIOEN(port);
    RCU->RCU_RSTDISAHB_reg |= RCU_RSTDISAHB_GPIOEN(port);

	return 0;
}

void niiet_uart_set_rcu(int num) {
    volatile uint32_t *rcu_clkcfg_reg = &RCU->RCU_UARTCLKCFG_reg[num];

	RCU->RCU_CGCFGAPB2_reg |= RCU_CGCFGAPB_UART_EN(num);
	RCU->RCU_RSTDISAPB2_reg |= RCU_RSTDISAPB_UART_EN(num);

    rcu_clkcfg_reg += num;

	*rcu_clkcfg_reg = 0;
	//*rcu_clkcfg_reg |= RCU_UARTCLKCFG_CLKSEL_SYSPLL0CLK_MASK;
    *rcu_clkcfg_reg |= RCU_UARTCLKCFG_CLKSEL_HSECLK_MASK;

	*rcu_clkcfg_reg |= RCU_UARTCLKCFG_CLKEN_MASK;
    *rcu_clkcfg_reg |= RCU_UARTCLKCFG_RSTDIS_MASK;
}


void niiet_tmr_set_rcu(int num) {
	RCU->RCU_CGCFGAPB1_reg |= RCU_CGCFGAPB_TMR_EN(num);
	RCU->RCU_RSTDISAPB1_reg |= RCU_RSTDISAPB_TMR_EN(num);
}

int clk_enable(char *clk_name) {
    int num;

    if (0 == strncmp(clk_name, CLK_NAME_GPIO, sizeof(CLK_NAME_GPIO) - 1)) {
        num = clk_name[sizeof(CLK_NAME_GPIO) - 1] - 'A';
        niiet_gpio_clock_setup(num);
        return 0;
    }
    if (0 == strncmp(clk_name, CLK_NAME_UART, sizeof(CLK_NAME_UART) - 1)) {
        num = clk_name[sizeof(CLK_NAME_UART) - 1]  - '0';
        niiet_uart_set_rcu(num);
        return 0;
    }
    if (0 == strncmp(clk_name, CLK_NAME_TMR, sizeof(CLK_NAME_TMR) - 1)) {
        num = clk_name[sizeof(CLK_NAME_TMR) - 1]  - '0';
        niiet_tmr_set_rcu(num);
        return 0;
    }

    return -ENOSUPP;
}

void niiet_sysclk_init(void) {
    uint32_t sysclk_source;
    uint32_t timeout_counter;
    
//select system clock
#ifdef CONF_RCU_TYPE_SYSCLK_PLL
    sysclk_source  = RCU_SYSCLKCFG_SRC_HSECLK; // (RCU_SYSCLKCFG_SRC_HSECLK << RCU_SYSCLKCFG_SRC_Pos);
    RCU->RCU_SYSCLKCFG_reg = sysclk_source;
    timeout_counter = 0;
    while (((RCU->RCU_CLKSTAT_reg & RCU_CLKSTAT_SRC_MASK) != sysclk_source) && (timeout_counter < 100)){ //SYSCLK_SWITCH_TIMEOUT))
        timeout_counter++;
    } 

//PLLCLK = REFCLK * (FBDIV+FRAC/2^24) / (REFDIV*(1+PD0A)*(1+PD0B))
#if (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 10000000)

#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 12000000)


#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 16000000)
    //FOUT = 120 000 000 Hz  from 16 MHz HSE
    // RCU->PLLDIV is equivalent for RCU->PLL[0].DIV
    RCU->RCU_PLLCFG_reg[0].DIV = ( 1 << RCU_PLL_DIV_DIV1A_Pos ) |
                  ( 1 << RCU_PLL_DIV_DIV1B_Pos ) |
                  ( 1 << RCU_PLL_DIV_PREDIV_Pos) |
                  ( 1 << RCU_PLL_DIV_NNCLR_Pos ) |             // N-divider enable
                  ( 1 << RCU_PLL_DIV_RNCLR_Pos ) |             // R-divider enable
                  ( 3 << RCU_PLL_DIV_RDIV_Pos  ) |
                  (120 << RCU_PLL_DIV_NDIV_Pos );
	RCU->RCU_PLLCFG_reg[0].MOD  = (1 << RCU_PLL_FRAC_FRAC_Pos );
	RCU->RCU_PLLCFG_reg[0].FRAC = (1 << RCU_PLL_MOD_MOD_Pos   );
	RCU->RCU_PLLCFG_reg[0].CFG  = (1 << RCU_PLL_CFG_FOUTEN_Pos ) |			// Fout enable
			       (3 << RCU_PLL_CFG_PFD_Pos    ) |
			       (0 << RCU_PLL_CFG_CLKSEL_Pos ) |
			       (1 << RCU_PLL_CFG_VCOMODE_Pos) |
			       (0 << RCU_PLL_CFG_ST_Pos) ;				  // ST = 0 for integer divider


#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 20000000)

#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 24000000)
#elif (CONF_RCU_CLK_ENABLE_HSECLK_VAL() == 27000000)
    //FOUT = 204 000 000 Hz  from 27 MHz HSE
    // RCU->PLLDIV is equivalent for RCU->PLL[0].DIV
    RCU->RCU_PLLCFG_reg[0].DIV = ( 1 << RCU_PLL_DIV_DIV1A_Pos ) |
                  ( 2 << RCU_PLL_DIV_DIV1B_Pos ) |
                  ( 1 << RCU_PLL_DIV_PREDIV_Pos) |
                  ( 1 << RCU_PLL_DIV_NNCLR_Pos ) |             // N-divider enable
                  ( 1 << RCU_PLL_DIV_RNCLR_Pos ) |             // R-divider enable
                  ( 2 << RCU_PLL_DIV_RDIV_Pos  ) |
                  (100 << RCU_PLL_DIV_NDIV_Pos );
	RCU->RCU_PLLCFG_reg[0].MOD  = (1 << RCU_PLL_FRAC_FRAC_Pos );
	RCU->RCU_PLLCFG_reg[0].FRAC = (1 << RCU_PLL_MOD_MOD_Pos   );
	RCU->RCU_PLLCFG_reg[0].CFG  = (1 << RCU_PLL_CFG_FOUTEN_Pos ) |			// Fout enable
			       (3 << RCU_PLL_CFG_PFD_Pos    ) |
			       (0 << RCU_PLL_CFG_CLKSEL_Pos ) |
			       (1 << RCU_PLL_CFG_VCOMODE_Pos) |
			       (0 << RCU_PLL_CFG_ST_Pos) ;				  // ST = 0 for integer divider
#else
#error "Please define HSECLK_VAL with correct values!"
#endif


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

    // FLASH control settings
    //FLASHM->CTRL_bit.LAT = 2;
    RCU->RCU_SYSCLKCFG_reg = sysclk_source;
    timeout_counter = 0;
    while (((RCU->RCU_CLKSTAT_reg & RCU_CLKSTAT_SRC_MASK) != sysclk_source) && (timeout_counter < 100)){ //SYSCLK_SWITCH_TIMEOUT))
        timeout_counter++;
    }
}
