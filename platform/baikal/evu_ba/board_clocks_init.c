/**
 * @file
 *
 * @date Sep 26, 2025
 * @author Anton Bondarev
 */

#include <hal/platform.h>
#include <baikal/evu_ba/bsp/bmcu_cru.h>

   

/**
 * @brief Makes a delay of the specified number of cycles.
 * @note The function uses RISC-V core performance monitoring counter to detect
 *       the delay timeout. It means that the function may fail if the delay 
 *       specified is close to the maximum value. On a 32-bit system the maximum
 *       value of the counter is limited to 32 bits to minimize error.
 * @note Can be executed in User mode or higher privilege mode.
 * @param cycles The number of cycles to delay.
 */
 __attribute__((always_inline)) static inline void __delay_cycles(unsigned long cycles)
{
    __asm volatile (
        "   rdcycle t0          \n"
        "1:                     \n"
        "   rdcycle t1          \n"
        "   sub t1, t1, t0      \n"
        "   bltu t1, %0, 1b     \n"
        :
        : "r"(cycles)
        : "t0", "t1", "memory"
    );
}



/* Register bitwise operations */
#define SET_BIT(REG, BIT)                       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)                     ((REG) &= ~(BIT))
#define WRITE_REG(REG, VAL)                     ((REG) = (VAL))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)     WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define READ_REG(REG)                           ((REG))




/** @brief Clock divider */
typedef enum {
    CRU_CLK_DIV_1 = 0,  /*!< Clock not divided */
    CRU_CLK_DIV_1_5,    /*!< Clock divided by 1.5 */
    CRU_CLK_DIV_2,      /*!< Clock divided by 2 */
    CRU_CLK_DIV_2_5,    /*!< Clock divided by 2.5 */
    CRU_CLK_DIV_3,      /*!< Clock divided by 3 */
    CRU_CLK_DIV_3_5,    /*!< Clock divided by 3.5 */
    CRU_CLK_DIV_4,      /*!< Clock divided by 4 */
    CRU_CLK_DIV_4_5,    /*!< Clock divided by 4.5 */
    CRU_CLK_DIV_5,      /*!< Clock divided by 5 */
    CRU_CLK_DIV_5_5,    /*!< Clock divided by 5.5 */
    CRU_CLK_DIV_6,      /*!< Clock divided by 6 */
    CRU_CLK_DIV_6_5,    /*!< Clock divided by 6.5 */
    CRU_CLK_DIV_7,      /*!< Clock divided by 7 */
    CRU_CLK_DIV_7_5,    /*!< Clock divided by 7.5 */
    CRU_CLK_DIV_8,      /*!< Clock divided by 8 */
    CRU_CLK_DIV_8_5,    /*!< Clock divided by 8.5 */
    CRU_CLK_DIV_9,      /*!< Clock divided by 9 */
    CRU_CLK_DIV_9_5,    /*!< Clock divided by 9.5 */
    CRU_CLK_DIV_10,     /*!< Clock divided by 10 */
    CRU_CLK_DIV_10_5,   /*!< Clock divided by 10.5 */
    CRU_CLK_DIV_11,     /*!< Clock divided by 11 */
    CRU_CLK_DIV_11_5,   /*!< Clock divided by 11.5 */
    CRU_CLK_DIV_12,     /*!< Clock divided by 12 */
    CRU_CLK_DIV_12_5,   /*!< Clock divided by 12.5 */
    CRU_CLK_DIV_13,     /*!< Clock divided by 13 */
    CRU_CLK_DIV_13_5,   /*!< Clock divided by 13.5 */
    CRU_CLK_DIV_14,     /*!< Clock divided by 14 */
    CRU_CLK_DIV_14_5,   /*!< Clock divided by 14.5 */
    CRU_CLK_DIV_15,     /*!< Clock divided by 15 */
    CRU_CLK_DIV_15_5,   /*!< Clock divided by 15.5 */
    CRU_CLK_DIV_16      /*!< Clock divided by 16 */
} CRU_ClkDiv_t;

/** @brief Clock source */
typedef enum {
    CRU_CLK_SRC_C0  = 0,    /*!< C0 clock selected as clock source */
    CRU_CLK_SRC_PLL,        /*!< PLL output selected as clock source */
    CRU_CLK_SRC_C1          /*!< C1 clock selected as clock source */
} CRU_ClkSrc_t;


/** @brief PLL source */
typedef enum {
    CRU_PLL_SRC_C0 = CRU_CLKSEL_REFCLKSEL_C0,   /*!< C0 clock selected as PLL clock source */
    CRU_PLL_SRC_C1 = CRU_CLKSEL_REFCLKSEL_C1    /*!< C1 clock selected as PLL clock source */
} CRU_PLLSrc_t;


/**
 * @brief Configures PLL source oscillator.
 * @param PLLSource The PLL source oscillator. Can be one of CRU_PLLSrc_t values.
 */
static inline void CRU_PLL_SetSource(CRU_PLLSrc_t PLLSource)
{
    MODIFY_REG(CRU->CLKSEL, CRU_CLKSEL_REFCLKSEL, (uint32_t)PLLSource);
}



/**
 * @brief Forces PLL reset.
 * @note In reset mode the PLL outputs a fixed free-running frequency
 *       in the range of 10MHz to 100MHz.
 */
static inline void CRU_PLL_ForceReset(void)
{
    SET_BIT(CRU->PLLSET, CRU_PLLSET_RESET);
}


/**
 * @brief Configures PLL.
 *        The configuration uses internal feedback. The output frequency
 *        is related to the reference frequency as Fout = Fref*NF/NR/OD.
 *        The recommended setting for NB is NF, which will yield the nominal
 *        bandwidth.
 * @param NR The PLL reference divider value. Must be in the range of 1 to 16.
 * @param NF The PLL multiplication factor value. Must be in the range of 1 to 64.
 * @param OD The PLL post VCO divider value. Must be in the range of 1 to 16.
 * @param NB The PLL bandwidth divider value. Must be in the range of 1 to 64.
 */
static inline void CRU_PLL_Config(uint32_t NR, uint32_t NF, uint32_t OD, uint32_t NB)
{
    MODIFY_REG(CRU->PLLSET,
               (CRU_PLLSET_BWADJ | CRU_PLLSET_CLKOD | CRU_PLLSET_CLKF | CRU_PLLSET_CLKR),
               (CRU_PLLSET_INTFB | ((NB-1) << CRU_PLLSET_BWADJ_Pos) | 
               ((OD-1) << CRU_PLLSET_CLKOD_Pos) | ((NF-1) << CRU_PLLSET_CLKF_Pos) |
               ((NR-1) << CRU_PLLSET_CLKR_Pos)));
}

/**
 * @brief Releases PLL reset.
 * @note In reset mode the PLL outputs a fixed free-running frequency
 *       in the range of 10MHz to 100MHz.
 */
static inline void CRU_PLL_ReleaseReset(void)
{
    CLEAR_BIT(CRU->PLLSET, CRU_PLLSET_RESET);
}

/**
 * @brief Sets CCLK prescaler.
 * @param Prescaler The CCLK prescaler. Can be one of CRU_ClkDiv_t values.
 */
static inline void CRU_SetCCLKPrescaler(CRU_ClkDiv_t Prescaler)
{
    MODIFY_REG(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_CCLKDIV, ((uint32_t)Prescaler) << CRU_SYSCLKSEL0_CCLKDIV_Pos);
}

/**
 * @brief Enables CCLK prescaler.
 */
static inline void CRU_CCLKPrescaler_Enable(void)
{
    SET_BIT(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_CCLKDIVEN);
}

/**
 * @brief Configures CCLK source.
 * @param Source The CCLK source. Can be one of CRU_ClkSrc_t values.
 */
static inline void CRU_SetCCLKSource(CRU_ClkSrc_t Source)
{
    MODIFY_REG(CRU->CLKSEL, CRU_CLKSEL_CCLKSEL, ((uint32_t)Source) << CRU_CLKSEL_CCLKSEL_Pos);
}

/**
 * @brief Sets PCLK0 prescaler.
 * @param Prescaler The PCLK0 prescaler. Can be one of CRU_ClkDiv_t values.
 */
static inline void CRU_SetPCLK0Prescaler(CRU_ClkDiv_t Prescaler)
{
    MODIFY_REG(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_PCLK0DIV, ((uint32_t)Prescaler) << CRU_SYSCLKSEL0_PCLK0DIV_Pos);
}


/**
 * @brief Enables PCLK0 prescaler.
 */
static inline void CRU_PCLK0Prescaler_Enable(void)
{
    SET_BIT(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_PCLK0DIVEN);
}

/**
 * @brief Configures PCLK0 source.
 * @param Source The PCLK0 source. Can be one of CRU_ClkSrc_t values.
 */
static inline void CRU_SetPCLK0Source(CRU_ClkSrc_t Source)
{
    MODIFY_REG(CRU->CLKSEL, CRU_CLKSEL_PCLK0SEL, ((uint32_t)Source) << CRU_CLKSEL_PCLK0SEL_Pos);
}



/**
 * @brief Sets PCLK1 prescaler.
 * @param Prescaler The PCLK1 prescaler. Can be one of CRU_ClkDiv_t values.
 */
static inline void CRU_SetPCLK1Prescaler(CRU_ClkDiv_t Prescaler)
{
    MODIFY_REG(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_PCLK1DIV, ((uint32_t)Prescaler) << CRU_SYSCLKSEL0_PCLK1DIV_Pos);
}

/**
 * @brief Enables PCLK1 prescaler.
 */
static inline void CRU_PCLK1Prescaler_Enable(void)
{
    SET_BIT(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_PCLK1DIVEN);
}


/**
 * @brief Configures PCLK1 source.
 * @param Source The PCLK1 source. Can be one of CRU_ClkSrc_t values.
 */
static inline void CRU_SetPCLK1Source(CRU_ClkSrc_t Source)
{
    MODIFY_REG(CRU->CLKSEL, CRU_CLKSEL_PCLK1SEL, ((uint32_t)Source) << CRU_CLKSEL_PCLK1SEL_Pos);
}


/**
 * @brief Sets PCLK2 prescaler.
 * @param Prescaler The PCLK2 prescaler. Can be one of CRU_ClkDiv_t values.
 */
static inline void CRU_SetPCLK2Prescaler(CRU_ClkDiv_t Prescaler)
{
    MODIFY_REG(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_PCLK2DIV, ((uint32_t)Prescaler) << CRU_SYSCLKSEL0_PCLK2DIV_Pos);
}


/**
 * @brief Enables PCLK2 prescaler.
 */
static inline void CRU_PCLK2Prescaler_Enable(void)
{
    SET_BIT(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_PCLK2DIVEN);
}


/**
 * @brief Configures PCLK2 source.
 * @param Source The PCLK2 source. Can be one of CRU_ClkSrc_t values.
 */
static inline void CRU_SetPCLK2Source(CRU_ClkSrc_t Source)
{
    MODIFY_REG(CRU->CLKSEL, CRU_CLKSEL_PCLK2SEL, ((uint32_t)Source) << CRU_CLKSEL_PCLK2SEL_Pos);
}

/**
 * @brief Sets HCLK prescaler.
 * @param Prescaler The HCLK prescaler. Can be one of CRU_ClkDiv_t values.
 */
static inline void CRU_SetHCLKPrescaler(CRU_ClkDiv_t Prescaler)
{
    MODIFY_REG(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_HCLKDIV, ((uint32_t)Prescaler) << CRU_SYSCLKSEL0_HCLKDIV_Pos);
}

/**
 * @brief Enables HCLK prescaler.
 */
static inline void CRU_HCLKPrescaler_Enable(void)
{
    SET_BIT(CRU->SYSCLKSEL0, CRU_SYSCLKSEL0_HCLKDIVEN);
}


/**
 * @brief Configures HCLK source.
 * @param Source The HCLK source. Can be one of CRU_ClkSrc_t values.
 */
static inline void CRU_SetHCLKSource(CRU_ClkSrc_t Source)
{
    MODIFY_REG(CRU->CLKSEL, CRU_CLKSEL_HCLKSEL, ((uint32_t)Source) << CRU_CLKSEL_HCLKSEL_Pos);
}



/**
 * @brief Sets CCLK 1MHz pulse prescaler.
 * @note The actual value of the prescaler in one more than the value written.
 * @note CCLK 1MHz pulse is used to drive CORE0 and CORE1 system timer.
 * @param Prescaler The prescaler value.
 */
static inline void CRU_SetCCLK1MHzPrescaler(uint32_t Prescaler)
{
    MODIFY_REG(CRU->CLKSEL, CRU_CLKSEL_CCLK1MHZ, (Prescaler << CRU_CLKSEL_CCLK1MHZ_Pos) & CRU_CLKSEL_CCLK1MHZ);
}

/**
 * @brief Sets HCLK 1MHz pulse prescaler.
 * @note The actual value of the prescaler in one more than the value written.
 * @note HCLK 1MHz pulse is used to drive CORE2 system timer.
 * @param Prescaler The prescaler value.
 */
static inline void CRU_SetHCLK1MHzPrescaler(uint32_t Prescaler)
{
    MODIFY_REG(CRU->CLKSEL, CRU_CLKSEL_HCLK1MHZ, (Prescaler << CRU_CLKSEL_HCLK1MHZ_Pos) & CRU_CLKSEL_HCLK1MHZ);
}


void board_clocks_init(void) {
	/*
     * Clock configuration:
     *
     * PLL = 200MHz
     * CCLK = 100MHz
     * PCLK0 = 100MHz
     * PCLK1 = 100MHz
     * PCLK2 = 100MHz
     * HCLK = 50MHz
     */

	/* Set PLL source clock to C0 */
	CRU_PLL_SetSource(CRU_PLL_SRC_C0);

	/* Configure PLL */
	CRU_PLL_ForceReset();
	CRU_PLL_Config(1UL, 48UL, 6UL, 48UL);
	CRU_PLL_ReleaseReset();

	/* PLL lock time min. 500 cycles */
	__delay_cycles(500UL);

	/* Configure CCLK */
	CRU_SetCCLKPrescaler(CRU_CLK_DIV_2);
	CRU_CCLKPrescaler_Enable();
	CRU_SetCCLKSource(CRU_CLK_SRC_PLL);

	/* Configure PCLK0 */
	CRU_SetPCLK0Prescaler(CRU_CLK_DIV_2);
	CRU_PCLK0Prescaler_Enable();
	CRU_SetPCLK0Source(CRU_CLK_SRC_PLL);

	/* Configure PCLK1 */
	CRU_SetPCLK1Prescaler(CRU_CLK_DIV_2);
	CRU_PCLK1Prescaler_Enable();
	CRU_SetPCLK1Source(CRU_CLK_SRC_PLL);

	/* Configure PCLK2 */
	CRU_SetPCLK2Prescaler(CRU_CLK_DIV_2);
	CRU_PCLK2Prescaler_Enable();
	CRU_SetPCLK2Source(CRU_CLK_SRC_PLL);

	/* Configure HCLK */
	CRU_SetHCLKPrescaler(CRU_CLK_DIV_4);
	CRU_HCLKPrescaler_Enable();
	CRU_SetHCLKSource(CRU_CLK_SRC_PLL);

	/* Configure 1MHz clocks */
	CRU_SetCCLK1MHzPrescaler(99UL);
	CRU_SetHCLK1MHzPrescaler(49UL);
}
