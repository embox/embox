

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

#include "mdr1206fi_rst_clk.h"


#undef USE_MDR32F02
#undef USE_MDR1206AFI

#define USE_MDR1206FI       1
#define USE_MDR1206         1



#define PERIPH_BASE   (0x50000000UL)

#define MDR_RST_CLK_BASE (PERIPH_BASE + 0x00020000UL)
#define MDR_BKP_BASE     (PERIPH_BASE + 0x00060000UL)

#define MDR_RST_CLK ((MDR_RST_CLK_TypeDef*)MDR_RST_CLK_BASE)

#define MDR_BKP     ((MDR_BKP_TypeDef*)MDR_BKP_BASE)

/**
 * @brief MDR32VF0xI RST_CLK control structure.
 */
typedef struct {
    volatile const uint32_t  CLOCK_STATUS; /*!<RST_CLK Clock Status Register */
    volatile uint32_t PLL_CONTROL;  /*!<RST_CLK PLL Control Register */
    volatile uint32_t HS_CONTROL;   /*!<RST_CLK HS Control Register */
    volatile uint32_t CPU_CLOCK;    /*!<RST_CLK CPU Clock Register */
    volatile uint32_t PER1_CLOCK;   /*!<RST_CLK PER1 Clock Register */
    volatile uint32_t ADC_CLOCK;    /*!<RST_CLK ADC Clock Register */
    volatile uint32_t RTC_CLOCK;    /*!<RST_CLK RTC Clock Register */
    volatile uint32_t PER2_CLOCK;   /*!<RST_CLK Peripheral Clock Enable Register */
#if defined(USE_MDR32F02_REV_1X)
    volatile const uint32_t RESERVED0; /*!<Reserved */
#else
    volatile uint32_t DMA_DONE_STICK; /*!<DMA_DONE Fixation Register */
#endif
    volatile uint32_t TIM_CLOCK;  /*!<RST_CLK Timer Clock Register */
    volatile uint32_t UART_CLOCK; /*!<RST_CLK UART Clock Register */
    volatile uint32_t SSP_CLOCK;  /*!<RST_CLK SSP Clock Register */
#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    volatile uint32_t DIV_SYS_TIM; /*!<Machine Timer Prescaler Register */
#endif
} MDR_RST_CLK_TypeDef;


/** @} */ /* End of group MDR32VF0xI_RST_CLK_PER2_CLOCK */

/**
 * @brief MDR32VF0xI BKP and RTC control structure.
 */
typedef struct {
    volatile uint32_t WPR;          /*!<BKP and RTC Protection Key Register */
    volatile uint32_t LDO;          /*!<BKP LDO Control Register */
    volatile uint32_t CLK;          /*!<BKP Clock Control Register */
    volatile uint32_t RTC_CR;       /*!<RTC Control Register */
    volatile uint32_t RTC_WUTR;     /*!<RTC Watchdog Counter Register */
    volatile uint32_t RTC_PREDIV_S; /*!<RTC Prescaler Counter Register */
    volatile uint32_t RTC_PRL;      /*!<RTC Prescaler Reload Value Register */
    volatile uint32_t RTC_ALRM;     /*!<RTC Alarm Register */
    volatile uint32_t RTC_CS;       /*!<RTC Control and Status Register */
    volatile uint32_t RTC_TR;       /*!<RTC Time Register */
    volatile uint32_t RTC_DR;       /*!<RTC Date Register */
    volatile uint32_t RTC_ALRMAR;   /*!<RTC Alarm A Reference Register */
    volatile uint32_t RTC_ALRMBR;   /*!<RTC Alarm B Reference Register */
    volatile const uint32_t  RTC_TSTR1;    /*!<RTC Timestamp1 Time Registerer */
    volatile const uint32_t  RTC_TSTR2;    /*!<RTC Timestamp2 Time Registerer */
    volatile const uint32_t  RTC_TSTR3;    /*!<RTC Timestamp3 Time Registerer */
    volatile const uint32_t  RTC_TSDR1;    /*!<RTC Timestamp1 Date Registerer */
    volatile const uint32_t  RTC_TSDR2;    /*!<RTC Timestamp2 Date Registerer */
    volatile const uint32_t  RTC_TSDR3;    /*!<RTC Timestamp3 Date Registerer */
    volatile uint32_t RTC_TAFCR;    /*!<RTC Tamper/Timestamp Control*/
#if defined(USE_MDR32F02)
    volatile const uint32_t RESERVED0[1004]; /*!<Reserved */
#elif defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    volatile uint32_t RTC_TMPCAL1;     /*!<RTC Temperature Compensation Calibration and Control Register 1 */
    volatile uint32_t RTC_TMPCAL2;     /*!<RTC Temperature Compensation Calibration and Control Register 2 */
    volatile uint32_t RTC_TMPCAL3;     /*!<RTC Temperature Compensation Calibration and Control Register 3 */
    volatile const uint32_t  RESERVED0[1001]; /*!<Reserved */
#endif
    volatile uint32_t MEM[128]; /*!<BKP Memory */
} MDR_BKP_TypeDef;

/** @defgroup MDR32VF0xI_BKP_WPR BKP WPR
 * @{
 */

#define BKP_WPR_BKP_WPR_Pos    0
#define BKP_WPR_BKP_WPR_Msk    (0xFFFFFFFFUL << BKP_WPR_BKP_WPR_Pos)
#define BKP_WPR_BKP_WPR_LOCK   (0x00000000UL)
#define BKP_WPR_BKP_WPR_UNLOCK (0x8555AAA1UL)


/** @defgroup MDR32VF0xI_BKP_CLK BKP CLK
 * @{
 */

#define BKP_CLK_LSE_ON_Pos       0
#define BKP_CLK_LSE_ON_Msk       (0x1UL << BKP_CLK_LSE_ON_Pos)
#define BKP_CLK_LSE_ON           BKP_CLK_LSE_ON_Msk

#define BKP_CLK_LSE_BYP_Pos      1
#define BKP_CLK_LSE_BYP_Msk      (0x1UL << BKP_CLK_LSE_BYP_Pos)
#define BKP_CLK_LSE_BYP          BKP_CLK_LSE_BYP_Msk

#define BKP_CLK_LSE_RDY_Pos      2
#define BKP_CLK_LSE_RDY_Msk      (0x1UL << BKP_CLK_LSE_RDY_Pos)
#define BKP_CLK_LSE_RDY          BKP_CLK_LSE_RDY_Msk

#define BKP_CLK_LSE_CONF_Pos     3
#define BKP_CLK_LSE_CONF_Msk     (0x3UL << BKP_CLK_LSE_CONF_Pos)

#define BKP_CLK_LSI_ON_Pos       5
#define BKP_CLK_LSI_ON_Msk       (0x1UL << BKP_CLK_LSI_ON_Pos)
#define BKP_CLK_LSI_ON           BKP_CLK_LSI_ON_Msk

#define BKP_CLK_LSI_RDY_Pos      12
#define BKP_CLK_LSI_RDY_Msk      (0x1UL << BKP_CLK_LSI_RDY_Pos)
#define BKP_CLK_LSI_RDY          BKP_CLK_LSI_RDY_Msk

#define BKP_CLK_HSI_ON_Pos       13
#define BKP_CLK_HSI_ON_Msk       (0x1UL << BKP_CLK_HSI_ON_Pos)
#define BKP_CLK_HSI_ON           BKP_CLK_HSI_ON_Msk

#define BKP_CLK_HSI_RDY_Pos      14
#define BKP_CLK_HSI_RDY_Msk      (0x1UL << BKP_CLK_HSI_RDY_Pos)
#define BKP_CLK_HSI_RDY          BKP_CLK_HSI_RDY_Msk

#define BKP_CLK_HSI_LSI_TRIM_Pos 15
#define BKP_CLK_HSI_LSI_TRIM_Msk (0x3FUL << BKP_CLK_HSI_LSI_TRIM_Pos)

/**
 * @brief  Set the RST_CLK clock configuration to the default reset state.
 * @param  None.
 * @return None.
 */
void RST_CLK_DeInit(void)
{
    
    uint32_t BKP_OldKey;

    /* Reset all peripheral clocks except RST_CLK and BKP_CLK. */
    MDR_RST_CLK->PER2_CLOCK = (RST_CLK_PER2_CLOCK_PCLK_EN_RST_CLK | RST_CLK_PER2_CLOCK_PCLK_EN_BKP);

    /* Read BKP key to variable. */
    BKP_OldKey = MDR_BKP->WPR;
    /* Write key to BKP_WPR to unlock access if key is not written. */
    if (BKP_OldKey != BKP_WPR_BKP_WPR_UNLOCK) {
        MDR_BKP->WPR = BKP_WPR_BKP_WPR_UNLOCK;
    }

    /* Enable HSI in BKP because RST_CLK deinitialization changes HCLK source to HSI. */
    MDR_BKP->CLK |= BKP_CLK_HSI_ON;

    /* Wait until HSI is ready. */
    while ((MDR_BKP->CLK & BKP_CLK_HSI_RDY) == 0) { }

    /* Lock BKP access if it was locked before function call. */
    if (BKP_OldKey != BKP_WPR_BKP_WPR_UNLOCK) {
        MDR_BKP->WPR = BKP_OldKey;
    }

    /* Reset all peripheral clocks except RST_CLK. */
    MDR_RST_CLK->PER2_CLOCK = RST_CLK_PER2_CLOCK_PCLK_EN_RST_CLK;

    MDR_RST_CLK->ADC_CLOCK = (uint32_t)0x00000000;
    MDR_RST_CLK->RTC_CLOCK = (uint32_t)0x00000000;

    /* Reset everything except DMA_EN bit. */
    MDR_RST_CLK->PER1_CLOCK = RST_CLK_PER1_CLOCK_DMA_EN;

    MDR_RST_CLK->TIM_CLOCK  = (uint32_t)0x00000000;
    MDR_RST_CLK->UART_CLOCK = (uint32_t)0x00000000;
    MDR_RST_CLK->SSP_CLOCK  = (uint32_t)0x00000000;

#if defined(USE_MDR32F02_REV_2) || defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    MDR_RST_CLK->DMA_DONE_STICK = (uint32_t)0x00000000;
    MDR_RST_CLK->DIV_SYS_TIM    = (uint32_t)0x00000000;
#endif

    /* Reset CPU_CLOCK bits. */
#if defined(USE_MDR32F02)
    /* HSI is selected as HCLK. */
    MDR_RST_CLK->CPU_CLOCK = (uint32_t)0x00000000;
#elif defined(USE_MDR1206FI) || defined(USE_MDR1206AFI) || defined(USE_MDR1206)
    /* CPU_C3 (from HSI) divided by 16 is selected as HCLK. */
    MDR_RST_CLK->CPU_CLOCK = ((uint32_t)RST_CLK_CPU_C3_PRESCALER_DIV_16 | RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3);
#endif
    MDR_RST_CLK->PLL_CONTROL = (uint32_t)0x00000000;
    MDR_RST_CLK->HS_CONTROL  = (uint32_t)0x00000000;
}

/**
 * @brief    Initialize the HCLK clock path according to
 *           the specified parameters in the RST_CLK_HCLK_InitStruct.
 * @note:    Necessary clock sources should be enabled before calling this function (as they are not configured by this function).
 * @note:    This function also updates SystemCoreClock variable.
 * @note:    The parameters that are not used in the selected path are not configurable.
 *           The only exception is PLL state which will turn off if not used for CPU_C2 (and CPU_C2 is used for HCLK).
 * @warning: If function failed to configure HCLK (due to PLL not ready), HCLK will fallback to HSI if HCLK was configured from CPU_C3 before.
 *           The only exception is PLL state which will turn off if not used for CPU_C2 (and CPU_C2 is used for HCLK).
 * @param    RST_CLK_HCLK_InitStruct: The pointer to the @ref RST_CLK_HCLK_InitTypeDef structure
 *           that contains the configuration information for the HCLK clock path.
 * @return   SUCCESS if HCLK clock configured, else ERROR.
 */
int RST_CLK_HCLK_Init(const RST_CLK_HCLK_InitTypeDef* RST_CLK_HCLK_InitStruct)
{
    uint32_t Temp1, Temp2;

    if (RST_CLK_HCLK_InitStruct->RST_CLK_HCLK_ClockSource == RST_CLK_CPU_HCLK_CLK_SRC_CPU_C3) {
        Temp1 = MDR_RST_CLK->CPU_CLOCK;
        if ((Temp1 & RST_CLK_CPU_CLOCK_HCLK_SEL_Msk) == RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3) {
            /* Switch HCLK from CPU_C3 to HSI to safely configure new HCLK clock path. */
            RST_CLK_HCLK_ClkSelection(RST_CLK_CPU_HCLK_CLK_SRC_HSI);
        }

        /* PLL is selected as new HCLK source. */
        if (RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C2_ClockSource == RST_CLK_CPU_C2_CLK_SRC_PLLCPU) {
            if ((MDR_RST_CLK->CPU_CLOCK & RST_CLK_CPU_CLOCK_CPU_C2_SEL_Msk) == RST_CLK_CPU_CLOCK_CPU_C2_SEL) {
                /* If PLL is source of CPU_C2, switch to CPU_C1 as CPU_C2 to safely (re)configure PLL. */
                RST_CLK_CPU_C2_ClkSelection(RST_CLK_CPU_C2_CLK_SRC_CPU_C1);
            }
            Temp2 = MDR_RST_CLK->PLL_CONTROL;
            if (RST_CLK_CPU_PLL_GetCmdState() == 1) {
                /* Disable PLL before reconfiguration. */
                RST_CLK_CPU_PLL_Cmd(0);
            }
            if (RST_CLK_HCLK_InitStruct->RST_CLK_PLLCPU_ClockSource == RST_CLK_PLLCPU_CLK_SRC_CPU_C1) {
                /* If PLL source will be CPU_C1, configure CPU_C1 source. */
                RST_CLK_CPU_C1_ClkSelection(RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C1_Source);
            }

            /* Configure PLL. */
            RST_CLK_CPU_PLL_Config(RST_CLK_HCLK_InitStruct->RST_CLK_PLLCPU_ClockSource, RST_CLK_HCLK_InitStruct->RST_CLK_PLLCPU_Multiplier);
            RST_CLK_CPU_PLL_Cmd(1);

            if (RST_CLK_CPU_PLL_GetStatus() == 0) {
                /* Restore previous PLL state. */
                MDR_RST_CLK->PLL_CONTROL = Temp2;

                if ((Temp1 & (RST_CLK_CPU_CLOCK_CPU_C2_SEL_Msk | RST_CLK_CPU_CLOCK_HCLK_SEL_Msk)) == (RST_CLK_CPU_CLOCK_CPU_C2_SEL | RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3) &&
                    RST_CLK_CPU_PLL_GetStatus() == 0) {
                    /* If PLL was used before and it is not ready again in its previous state, fallback to HSI. */
                    MDR_RST_CLK->CPU_CLOCK = Temp1 & ~RST_CLK_CPU_CLOCK_HCLK_SEL_Msk;
                } else {
                    /* Restore previous HCLK state. */
                    MDR_RST_CLK->CPU_CLOCK = Temp1;
                }
                return 0;
            }
        }

        RST_CLK_CPU_C2_ClkSelection(RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C2_ClockSource);
        RST_CLK_CPU_C3_SetPrescaler(RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C3_Prescaler);
        RST_CLK_HCLK_ClkSelection(RST_CLK_HCLK_InitStruct->RST_CLK_HCLK_ClockSource);
    } else {
        RST_CLK_HCLK_ClkSelection(RST_CLK_HCLK_InitStruct->RST_CLK_HCLK_ClockSource);
    }

    return 1;
}

/**
 * @brief  HSE (High Speed External) clock mode and source selection based on @ref HSE_EXT_GEN_MODE definition.
 * @param  NewState: a new state of the HSE clock.
 * @return None.
 */
void RST_CLK_HSE_Cmd(int NewState)
{

    if (NewState != 0) {
#if !defined(HSE_EXT_GEN_MODE)
        MDR_RST_CLK->HS_CONTROL = (MDR_RST_CLK->HS_CONTROL & ~RST_CLK_HS_CONTROL_HSE_BYP) | RST_CLK_HS_CONTROL_HSE_ON;
#else
        MDR_RST_CLK->HS_CONTROL |= (RST_CLK_HS_CONTROL_HSE_BYP | RST_CLK_HS_CONTROL_HSE_ON);
#endif
    } else {
#if !defined(HSE_EXT_GEN_MODE)
        MDR_RST_CLK->HS_CONTROL &= ~(RST_CLK_HS_CONTROL_HSE_BYP | RST_CLK_HS_CONTROL_HSE_ON);
#else
        MDR_RST_CLK->HS_CONTROL = (MDR_RST_CLK->HS_CONTROL | RST_CLK_HS_CONTROL_HSE_BYP) & ~RST_CLK_HS_CONTROL_HSE_ON;
#endif
    }
}

/**
 * @brief  Get HSE clock status.
 * @note   The cycle counter and core clock from SystemCoreClock are used for delay. Do not disable the cycle counter.
 *         Before calling this function, you must update SystemCoreClock by calling SystemCoreClockUpdate().
 *         If the HSERDY_USER_TIMEOUT_MS definition is active, the start time for MDR1206 is calculated based on the resonator characteristics.
 * @param  None.
 * @return SUCCESS if HSE clock is ready, else ERROR.
 */
int RST_CLK_HSE_GetStatus(void)
{

    int State;
    int  Flag;

    for (volatile int cnt =0; cnt < 10000; cnt ++) {

    }

    Flag = RST_CLK_GetFlagStatus(RST_CLK_FLAG_HSERDY);

    if (Flag != 0) {
        State = 1;
    } else {
        State = 0;
    }

    return State;
}

/**
 * @brief  Configure the CPU_PLL clock source and multiplication factor.
 * @param  PLLCPU_Source: @ref RST_CLK_PLLCPU_ClockSource_TypeDef - specify the PLL entry clock source.
 * @param  PLLCPU_Multiplier: @ref RST_CLK_PLLCPU_Multiplier_TypeDef - specify the PLL multiplication factor.
 * @return None.
 */
void RST_CLK_CPU_PLL_Config(RST_CLK_PLLCPU_ClockSource_TypeDef PLLCPU_Source, uint32_t PLLCPU_Multiplier)
{
    uint32_t Temp;


    /* Select CPUPLL source. */
    Temp = MDR_RST_CLK->PLL_CONTROL;
    Temp &= ~RST_CLK_PLL_CONTROL_PLL_CPU_SEL_Msk;
    Temp |= (uint32_t)PLLCPU_Source;

    /* Set CPUPLL multiplier. */
    /* Clear PLLMUL[2:0] bits. */
    Temp &= ~RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Msk;
    /* Set PLLMUL[2:0] bits. */
    Temp |= (PLLCPU_Multiplier << RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos);
    /* Store the new value. */
    MDR_RST_CLK->PLL_CONTROL = Temp;
}

/**
 * @brief  Enable or disable the CPU_PLL clock.
 * @param  NewState: - a new state of the CPU_PLL clock.
 * @return None.
 */
void RST_CLK_CPU_PLL_Cmd(int NewState)
{
    uint32_t Temp;


    Temp = MDR_RST_CLK->PLL_CONTROL;

    if (NewState != 0) {
        Temp |= RST_CLK_PLL_CONTROL_PLL_CPU_ON;
    } else {
        Temp &= ~RST_CLK_PLL_CONTROL_PLL_CPU_ON;
    }

    MDR_RST_CLK->PLL_CONTROL = Temp;
}

/**
 * @brief  Get the CPU_PLL clock work state.
 * @param  None.
 * @return the current work state of the CPU_PLL clock.
 */
int RST_CLK_CPU_PLL_GetCmdState(void)
{
    int State;

    if ((MDR_RST_CLK->PLL_CONTROL & RST_CLK_PLL_CONTROL_PLL_CPU_ON) == 0) {
        State = 0;
    } else {
        State = 1;
    }

    return State;
}

/**
 * @brief  Get CPU_PLL clock status.
 * @note   The cycle counter and core clock from SystemCoreClock are used for delay. Do not disable the cycle counter.
 *         Before calling this function, you must update SystemCoreClock by calling SystemCoreClockUpdate().
 * @param  None.
 * @return SUCCESS if CPU_PLL output clock is ready, else ERROR.
 */
int RST_CLK_CPU_PLL_GetStatus(void)
{

    int State;
    int  Flag;

    for (volatile int cnt =0; cnt < 10000; cnt ++) {

    }


    Flag = RST_CLK_GetFlagStatus(RST_CLK_FLAG_PLLCPURDY);

    if (Flag != 0) {
        State = 1;
    } else {
        State = 0;
    }

    return State;
}

/**
 * @brief  Select the CPU_C1 clock source.
 * @param  CPU_C1_Source: @ref RST_CLK_CPU_C1_ClockSource_TypeDef - specify the CPU_C1 clock source.
 * @return None.
 */
void RST_CLK_CPU_C1_ClkSelection(RST_CLK_CPU_C1_ClockSource_TypeDef CPU_C1_Source)
{
    uint32_t Temp = 0;


    Temp = MDR_RST_CLK->CPU_CLOCK;

    /* Clear CPU_C1_SEL bits. */
    Temp &= ~RST_CLK_CPU_CLOCK_CPU_C1_SEL_Msk;

    /* Set the CPU_C1_SEL bits. */
    Temp |= (uint32_t)CPU_C1_Source;

    /* Store the new value. */
    MDR_RST_CLK->CPU_CLOCK = Temp;
}

/**
 * @brief  Select the CPU_C2 clock source.
 * @param  CPU_C2_Source: @ref RST_CLK_CPU_C2_ClockSource_TypeDef - specify the CPU_C2 clock source.
 * @return None.
 */
void RST_CLK_CPU_C2_ClkSelection(RST_CLK_CPU_C2_ClockSource_TypeDef CPU_C2_Source)
{
    uint32_t Temp = 0;


    Temp = MDR_RST_CLK->CPU_CLOCK;
    Temp &= ~RST_CLK_CPU_CLOCK_CPU_C2_SEL;
    Temp |= (uint32_t)CPU_C2_Source;

    /* Configure CPU_CLOCK register with new value. */
    MDR_RST_CLK->CPU_CLOCK = Temp;
}

/**
 * @brief  Configure the CPU_C3_SEL division factor.
 * @param  CPU_CLK_DivValue: @ref RST_CLK_CPU_C3_Prescaler_TypeDef - specify the CPU_C3_SEL division factor.
 * @return None.
 */
void RST_CLK_CPU_C3_SetPrescaler(RST_CLK_CPU_C3_Prescaler_TypeDef CPU_CLK_DivValue)
{
    uint32_t Temp;


    Temp = MDR_RST_CLK->CPU_CLOCK;

    /* Clear CPU_C3_SEL bits. */
    Temp &= ~RST_CLK_CPU_CLOCK_CPU_C3_SEL_Msk;

    /* Set CPU_C3_SEL bits. */
    Temp |= (uint32_t)CPU_CLK_DivValue;

    /* Store the new value. */
    MDR_RST_CLK->CPU_CLOCK = Temp;
}

/**
 * @brief  Select the HCLK clock source.
 * @param  CPU_HCLK_Source: @ref RST_CLK_HCLK_ClockSource_TypeDef - specify the HCLK clock source.
 * @return None.
 */
void RST_CLK_HCLK_ClkSelection(RST_CLK_HCLK_ClockSource_TypeDef CPU_HCLK_Source)
{
    uint32_t Temp;

    Temp = MDR_RST_CLK->CPU_CLOCK;

    /* Clear CPU_C3_SEL bits. */
    Temp &= ~RST_CLK_CPU_CLOCK_HCLK_SEL_Msk;

    /* Set the CPU_C3_SEL bits. */
    Temp |= (uint32_t)CPU_HCLK_Source;

    /* Store the new value. */
    MDR_RST_CLK->CPU_CLOCK = Temp;
}

/**
 * @brief  Check whether a specified RST flag is set or not.
 * @param  RST_CLK_Flag: @ref RST_CLK_Flags_TypeDef - specify a flag to check.
 * @return  the state of clock ready flag (SET or RESET).
 */
int RST_CLK_GetFlagStatus(RST_CLK_Flags_TypeDef RST_CLK_Flag)
{
    int Status;

    /* Get the flag status on a proper position. */
    if ((MDR_RST_CLK->CLOCK_STATUS & (uint32_t)RST_CLK_Flag) != 0) {
        Status = 1;
    } else {
        Status = 0;
    }

    /* Return the flag status. */
    return Status;
}
