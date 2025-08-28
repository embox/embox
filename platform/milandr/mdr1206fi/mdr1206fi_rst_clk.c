/**
 *******************************************************************************
 * @file    MDR32VF0xI_rst_clk.c
 * @author  Milandr Application Team
 * @version V0.2.0
 * @date    23/05/2025
 * @brief   This file contains all the RST_CLK firmware functions.
 *******************************************************************************
 * <br><br>
 *
 * THE PRESENT FIRMWARE IS FOR GUIDANCE ONLY. IT AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING MILANDR'S PRODUCTS IN ORDER TO FACILITATE
 * THE USE AND SAVE TIME. MILANDR SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR A USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2025 Milandr</center></h2>
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "rst_clk.h"
#include "mdr1206fi.h"

extern void SystemCoreClockUpdate(void);

/** @addtogroup MDR32VF0xI_StdPeriph_Driver MDR32VF0xI Standard Peripheral Driver
 * @{
 */

/** @defgroup RST_CLK RST_CLK
 * @{
 */

/** @defgroup RST_CLK_Exported_Functions RST_CLK Exported Functions
 * @{
 */

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
    while ((MDR_BKP->CLK & BKP_CLK_HSI_RDY) == RESET) { }

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
 * @return   @ref ErrorStatus - SUCCESS if HCLK clock configured, else ERROR.
 */
ErrorStatus RST_CLK_HCLK_Init(const RST_CLK_HCLK_InitTypeDef* RST_CLK_HCLK_InitStruct)
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
            if (RST_CLK_CPU_PLL_GetCmdState() == ENABLE) {
                /* Disable PLL before reconfiguration. */
                RST_CLK_CPU_PLL_Cmd(DISABLE);
            }
            if (RST_CLK_HCLK_InitStruct->RST_CLK_PLLCPU_ClockSource == RST_CLK_PLLCPU_CLK_SRC_CPU_C1) {
                /* If PLL source will be CPU_C1, configure CPU_C1 source. */
                RST_CLK_CPU_C1_ClkSelection(RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C1_Source);
            }

            /* Configure PLL. */
            RST_CLK_CPU_PLL_Config(RST_CLK_HCLK_InitStruct->RST_CLK_PLLCPU_ClockSource, RST_CLK_HCLK_InitStruct->RST_CLK_PLLCPU_Multiplier);
            RST_CLK_CPU_PLL_Cmd(ENABLE);
            SystemCoreClockUpdate();
            if (RST_CLK_CPU_PLL_GetStatus() == ERROR) {
                /* Restore previous PLL state. */
                MDR_RST_CLK->PLL_CONTROL = Temp2;
                SystemCoreClockUpdate();
                if ((Temp1 & (RST_CLK_CPU_CLOCK_CPU_C2_SEL_Msk | RST_CLK_CPU_CLOCK_HCLK_SEL_Msk)) == (RST_CLK_CPU_CLOCK_CPU_C2_SEL | RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3) &&
                    RST_CLK_CPU_PLL_GetStatus() == ERROR) {
                    /* If PLL was used before and it is not ready again in its previous state, fallback to HSI. */
                    MDR_RST_CLK->CPU_CLOCK = Temp1 & ~RST_CLK_CPU_CLOCK_HCLK_SEL_Msk;
                } else {
                    /* Restore previous HCLK state. */
                    MDR_RST_CLK->CPU_CLOCK = Temp1;
                }
                return ERROR;
            }
        }

        RST_CLK_CPU_C2_ClkSelection(RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C2_ClockSource);
        RST_CLK_CPU_C3_SetPrescaler(RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C3_Prescaler);
        RST_CLK_HCLK_ClkSelection(RST_CLK_HCLK_InitStruct->RST_CLK_HCLK_ClockSource);
    } else {
        RST_CLK_HCLK_ClkSelection(RST_CLK_HCLK_InitStruct->RST_CLK_HCLK_ClockSource);
    }
    SystemCoreClockUpdate();
    return SUCCESS;
}

/**
 * @brief  Fill each RST_CLK_HCLK_InitTypeDef member with its default (i.e. reset) value.
 * @param  RST_CLK_HCLK_InitStruct: The pointer to the @ref RST_CLK_HCLK_InitTypeDef structure
 *         which is to be initialized.
 * @return None.
 */
void RST_CLK_HCLK_StructInit(RST_CLK_HCLK_InitTypeDef* RST_CLK_HCLK_InitStruct)
{
    RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C1_Source      = RST_CLK_CPU_C1_CLK_SRC_HSI;
    RST_CLK_HCLK_InitStruct->RST_CLK_PLLCPU_ClockSource = RST_CLK_PLLCPU_CLK_SRC_CPU_C1;
    RST_CLK_HCLK_InitStruct->RST_CLK_PLLCPU_Multiplier  = RST_CLK_PLLCPU_MUL_1;
    RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C2_ClockSource = RST_CLK_CPU_C2_CLK_SRC_CPU_C1;
#if defined(USE_MDR32F02)
    RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C3_Prescaler = RST_CLK_CPU_C3_PRESCALER_DIV_1;
    RST_CLK_HCLK_InitStruct->RST_CLK_HCLK_ClockSource = RST_CLK_CPU_HCLK_CLK_SRC_HSI;
#else
    RST_CLK_HCLK_InitStruct->RST_CLK_CPU_C3_Prescaler = RST_CLK_CPU_C3_PRESCALER_DIV_16;
    RST_CLK_HCLK_InitStruct->RST_CLK_HCLK_ClockSource = RST_CLK_CPU_HCLK_CLK_SRC_CPU_C3;
#endif
}

/**
 * @brief  HSE (High Speed External) clock mode and source selection based on @ref HSE_EXT_GEN_MODE definition.
 * @param  NewState: @ref FunctionalState - a new state of the HSE clock.
 * @return None.
 */
void RST_CLK_HSE_Cmd(FunctionalState NewState)
{

    if (NewState != DISABLE) {
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
 * @brief  HSE (High Speed External) clock mode and source selection.
 * @param  HSE_Mode: @ref RST_CLK_HSE_Mode_TypeDef - mode selector.
 * @return None.
 */
void RST_CLK_HSE_Config(RST_CLK_HSE_Mode_TypeDef HSE_Mode)
{

    /* Reset HSEON and HSEBYP bits before configuring the HSE */
    MDR_RST_CLK->HS_CONTROL &= ~(RST_CLK_HS_CONTROL_HSE_ON | RST_CLK_HS_CONTROL_HSE_BYP);

    MDR_RST_CLK->HS_CONTROL |= (uint32_t)HSE_Mode;
}

/**
 * @brief  Get the HSE clock mode and source selection.
 * @param  None.
 * @return @ref RST_CLK_HSE_Mode_TypeDef - current HSE mode mode and source selection.
 */
RST_CLK_HSE_Mode_TypeDef RST_CLK_HSE_GetModeConfig(void)
{
    return (RST_CLK_HSE_Mode_TypeDef)(MDR_RST_CLK->HS_CONTROL &
                                      (RST_CLK_HS_CONTROL_HSE_ON | RST_CLK_HS_CONTROL_HSE_BYP));
}

/**
 * @brief  Get HSE clock status.
 * @note   The cycle counter and core clock from SystemCoreClock are used for delay. Do not disable the cycle counter.
 *         Before calling this function, you must update SystemCoreClock by calling SystemCoreClockUpdate().
 *         If the HSERDY_USER_TIMEOUT_MS definition is active, the start time for MDR1206 is calculated based on the resonator characteristics.
 * @param  None.
 * @return @ref ErrorStatus - SUCCESS if HSE clock is ready, else ERROR.
 */
ErrorStatus RST_CLK_HSE_GetStatus(void)
{

    ErrorStatus State;
    FlagStatus  Flag;

    for (volatile int cnt =0; cnt < 10000; cnt ++) {

    }

    Flag = RST_CLK_GetFlagStatus(RST_CLK_FLAG_HSERDY);

    if (Flag != RESET) {
        State = SUCCESS;
    } else {
        State = ERROR;
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
 * @param  NewState: @ref FunctionalState - a new state of the CPU_PLL clock.
 * @return None.
 */
void RST_CLK_CPU_PLL_Cmd(FunctionalState NewState)
{
    uint32_t Temp;


    Temp = MDR_RST_CLK->PLL_CONTROL;

    if (NewState != DISABLE) {
        Temp |= RST_CLK_PLL_CONTROL_PLL_CPU_ON;
    } else {
        Temp &= ~RST_CLK_PLL_CONTROL_PLL_CPU_ON;
    }

    MDR_RST_CLK->PLL_CONTROL = Temp;
}

/**
 * @brief  Get the CPU_PLL clock work state.
 * @param  None.
 * @return @ref FunctionalState - the current work state of the CPU_PLL clock.
 */
FunctionalState RST_CLK_CPU_PLL_GetCmdState(void)
{
    FunctionalState State;

    if ((MDR_RST_CLK->PLL_CONTROL & RST_CLK_PLL_CONTROL_PLL_CPU_ON) == 0) {
        State = DISABLE;
    } else {
        State = ENABLE;
    }

    return State;
}

/**
 * @brief  Get CPU_PLL clock status.
 * @note   The cycle counter and core clock from SystemCoreClock are used for delay. Do not disable the cycle counter.
 *         Before calling this function, you must update SystemCoreClock by calling SystemCoreClockUpdate().
 * @param  None.
 * @return @ref ErrorStatus - SUCCESS if CPU_PLL output clock is ready, else ERROR.
 */
ErrorStatus RST_CLK_CPU_PLL_GetStatus(void)
{

    ErrorStatus State;
    FlagStatus  Flag;

    for (volatile int cnt =0; cnt < 10000; cnt ++) {

    }


    Flag = RST_CLK_GetFlagStatus(RST_CLK_FLAG_PLLCPURDY);

    if (Flag != RESET) {
        State = SUCCESS;
    } else {
        State = ERROR;
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
 * @brief  Configure the HSI_C1_SEL division factor.
 * @param  HSI_CLK_DivValue: @ref RST_CLK_HSI_C1_Prescaler_TypeDef - specify the HSI_C1_SEL division factor.
 * @return None.
 */
void RST_CLK_HSI_C1_SetPrescaler(RST_CLK_HSI_C1_Prescaler_TypeDef HSI_CLK_DivValue)
{
    uint32_t Temp;


    Temp = MDR_RST_CLK->RTC_CLOCK;

    /* Clear HSI_SEL bits. */
    Temp &= ~RST_CLK_RTC_CLOCK_HSI_SEL_Msk;

    /* Set HSI_SEL bits. */
    Temp |= (uint32_t)HSI_CLK_DivValue;

    /* Store the new value. */
    MDR_RST_CLK->RTC_CLOCK = Temp;
}

/**
 * @brief  Enable or disable the RTCHSI clock.
 * @param  NewState: @ref FunctionalState - a new state of the RTCHSI clock.
 * @return None.
 */
void RST_CLK_RTC_HSI_ClkCmd(FunctionalState NewState)
{

    if (NewState != DISABLE) {
        /* Enable HSI RTC clock by setting the HSE_RTC_EN bit in the RTC_CLOCK register. */
        MDR_RST_CLK->RTC_CLOCK |= RST_CLK_RTC_CLOCK_HSI_RTC_EN;
    } else {
        /* Disable HSI RTC clock by resetting the HSE_RTC_EN bit in the RTC_CLOCK register. */
        MDR_RST_CLK->RTC_CLOCK &= ~RST_CLK_RTC_CLOCK_HSI_RTC_EN;
    }
}

/**
 * @brief  Get the RTCHSI clock work state.
 * @param  None.
 * @return @ref FunctionalState - the current work state of the RTCHSI clock.
 */
FunctionalState RST_CLK_RTC_HSI_GetClkCmdState(void)
{
    FunctionalState State;

    if ((MDR_RST_CLK->RTC_CLOCK & RST_CLK_RTC_CLOCK_HSI_RTC_EN) == 0) {
        State = DISABLE;
    } else {
        State = ENABLE;
    }

    return State;
}

/**
 * @brief  Configure the HSE_C1_SEL division factor.
 * @param  HSE_CLK_DivValue: @ref RST_CLK_HSE_C1_Prescaler_TypeDef - specify the HSE_C1_SEL division factor.
 * @return None.
 */
void RST_CLK_HSE_C1_SetPrescaler(RST_CLK_HSE_C1_Prescaler_TypeDef HSE_CLK_DivValue)
{
    uint32_t Temp;

    Temp = MDR_RST_CLK->RTC_CLOCK;

    /* Clear HSE_SEL bits. */
    Temp &= ~RST_CLK_RTC_CLOCK_HSE_SEL_Msk;

    /* Set HSE_SEL bits. */
    Temp |= (uint32_t)HSE_CLK_DivValue;

    /* Store the new value. */
    MDR_RST_CLK->RTC_CLOCK = Temp;
}

/**
 * @brief  Enable or disable the RTCHSE clock.
 * @param  NewState: @ref FunctionalState - a new state of the HSE_RTC_EN bit.
 * @return None.
 */
void RST_CLK_RTC_HSE_ClkCmd(FunctionalState NewState)
{

    if (NewState != DISABLE) {
        /* Enable HSI RTC clock by setting the HSE_RTC_EN bit in the RTC_CLOCK register. */
        MDR_RST_CLK->RTC_CLOCK |= RST_CLK_RTC_CLOCK_HSE_RTC_EN;
    } else {
        /* Disable HSI RTC clock by resetting the HSE_RTC_EN bit in the RTC_CLOCK register. */
        MDR_RST_CLK->RTC_CLOCK &= ~RST_CLK_RTC_CLOCK_HSE_RTC_EN;
    }
}

/**
 * @brief  Get the RTCHSE clock work state.
 * @param  None.
 * @return @ref FunctionalState - the current work state of the RTCHSE clock.
 */
FunctionalState RST_CLK_RTC_HSE_GetClkCmdState(void)
{
    FunctionalState State;

    if ((MDR_RST_CLK->RTC_CLOCK & RST_CLK_RTC_CLOCK_HSE_RTC_EN) == 0) {
        State = DISABLE;
    } else {
        State = ENABLE;
    }

    return State;
}

/**
 * @brief  Select a clock source for PER1_C1.
 * @param  PER1_C1_Source: @ref RST_CLK_PER1_C1_ClockSource_TypeDef - specify PER1_C1 clock source.
 * @return None.
 */
void RST_CLK_PER1_C1_ClkSelection(RST_CLK_PER1_C1_ClockSource_TypeDef PER1_C1_Source)
{

    /* Clear an old clock source and set a new one. */
    MDR_RST_CLK->PER1_CLOCK = (MDR_RST_CLK->PER1_CLOCK & (~RST_CLK_PER1_CLOCK_PER1_C1_SEL_Msk)) |
                              (uint32_t)PER1_C1_Source;
}

/**
 * @brief  Select a clock source for PER1_C2.
 * @param  PER1_C2_Source: @ref RST_CLK_PER1_C2_ClockSource_TypeDef - specify PER1_C2 clock source.
 * @return None.
 */
void RST_CLK_PER1_C2_ClkSelection(RST_CLK_PER1_C2_ClockSource_TypeDef PER1_C2_Source)
{

    MDR_RST_CLK->PER1_CLOCK = (MDR_RST_CLK->PER1_CLOCK & (~RST_CLK_PER1_CLOCK_PER1_C2_SEL_Msk)) |
                              (uint32_t)PER1_C2_Source;
}

/**
 * @brief  Configure division factor for @ref RST_CLK_PER1_C2_X_TypeDef peripheral clock.
 * @param  PER1_CLK: @ref RST_CLK_PER1_C2_X_TypeDef - specify peripheral clock.
 * @param  PER1_C2_DivValue: @ref RST_CLK_PER1_C2_Prescaler_TypeDef - specify a prescaler.
 * @return None.
 */
void RST_CLK_PER1_C2_SetPrescaler(RST_CLK_PER1_C2_X_TypeDef PER1_CLK, RST_CLK_PER1_C2_Prescaler_TypeDef PER1_C2_DivValue)
{

    switch (PER1_CLK) {
        case RST_CLK_PER1_C2_UART1:
            MDR_RST_CLK->UART_CLOCK = (MDR_RST_CLK->UART_CLOCK & (~RST_CLK_UART_CLOCK_UART1_BRG_Msk)) |
                                      ((uint32_t)PER1_C2_DivValue << RST_CLK_UART_CLOCK_UART1_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_UART2:
            MDR_RST_CLK->UART_CLOCK = (MDR_RST_CLK->UART_CLOCK & (~RST_CLK_UART_CLOCK_UART2_BRG_Msk)) |
                                      ((uint32_t)PER1_C2_DivValue << RST_CLK_UART_CLOCK_UART2_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_UART3:
            MDR_RST_CLK->UART_CLOCK = (MDR_RST_CLK->UART_CLOCK & (~RST_CLK_UART_CLOCK_UART3_BRG_Msk)) |
                                      ((uint32_t)PER1_C2_DivValue << RST_CLK_UART_CLOCK_UART3_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_UART4:
            MDR_RST_CLK->UART_CLOCK = (MDR_RST_CLK->UART_CLOCK & (~RST_CLK_UART_CLOCK_UART4_BRG_Msk)) |
                                      ((uint32_t)PER1_C2_DivValue << RST_CLK_UART_CLOCK_UART4_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_TIM1:
            MDR_RST_CLK->TIM_CLOCK = (MDR_RST_CLK->TIM_CLOCK & (~RST_CLK_TIM_CLOCK_TIM1_BRG_Msk)) |
                                     ((uint32_t)PER1_C2_DivValue << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_TIM2:
            MDR_RST_CLK->TIM_CLOCK = (MDR_RST_CLK->TIM_CLOCK & (~RST_CLK_TIM_CLOCK_TIM2_BRG_Msk)) |
                                     ((uint32_t)PER1_C2_DivValue << RST_CLK_TIM_CLOCK_TIM2_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_TIM3:
            MDR_RST_CLK->TIM_CLOCK = (MDR_RST_CLK->TIM_CLOCK & (~RST_CLK_TIM_CLOCK_TIM3_BRG_Msk)) |
                                     ((uint32_t)PER1_C2_DivValue << RST_CLK_TIM_CLOCK_TIM3_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_TIM4:
            MDR_RST_CLK->TIM_CLOCK = (MDR_RST_CLK->TIM_CLOCK & (~RST_CLK_TIM_CLOCK_TIM4_BRG_Msk)) |
                                     ((uint32_t)PER1_C2_DivValue << RST_CLK_TIM_CLOCK_TIM4_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_SSP1:
            MDR_RST_CLK->SSP_CLOCK = (MDR_RST_CLK->SSP_CLOCK & (~RST_CLK_SSP_CLOCK_SSP1_BRG_Msk)) |
                                     ((uint32_t)PER1_C2_DivValue << RST_CLK_SSP_CLOCK_SSP1_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_SSP2:
            MDR_RST_CLK->SSP_CLOCK = (MDR_RST_CLK->SSP_CLOCK & (~RST_CLK_SSP_CLOCK_SSP2_BRG_Msk)) |
                                     ((uint32_t)PER1_C2_DivValue << RST_CLK_SSP_CLOCK_SSP2_BRG_Pos);
            break;
        case RST_CLK_PER1_C2_SSP3:
            MDR_RST_CLK->SSP_CLOCK = (MDR_RST_CLK->SSP_CLOCK & (~RST_CLK_SSP_CLOCK_SSP3_BRG_Msk)) |
                                     ((uint32_t)PER1_C2_DivValue << RST_CLK_SSP_CLOCK_SSP3_BRG_Pos);
            break;
    }
}

/**
 * @brief  Enable or disable @ref RST_CLK_PER1_C2_X_TypeDef peripheral clock.
 * @param  PER1_CLK: Specify peripheral clock.
 *         This parameter can be any combination of @ref RST_CLK_PER1_C2_X_TypeDef values.
 * @param  NewState: @ref FunctionalState - a new state of the peripheral clock.
 * @return None.
 */
void RST_CLK_PER1_C2_Cmd(uint32_t PER1_CLK, FunctionalState NewState)
{
    uint32_t Temp;


    /* Handle UARTx. */
    Temp = PER1_CLK & RST_CLK_PER1_CLK_UART_MASK;
    if (Temp != 0) {
        if (NewState != DISABLE) {
            MDR_RST_CLK->UART_CLOCK |= (Temp << RST_CLK_UART_CLOCK_UART1_CLK_EN_Pos);
        } else {
            MDR_RST_CLK->UART_CLOCK &= ~(Temp << RST_CLK_UART_CLOCK_UART1_CLK_EN_Pos);
        }
    }

    /* Handle TIMERx. */
    Temp = PER1_CLK & RST_CLK_PER1_CLK_TIM_MASK;
    if (Temp != 0) {
        Temp >>= 7;
        if (NewState != DISABLE) {
            MDR_RST_CLK->TIM_CLOCK |= (Temp << RST_CLK_TIM_CLOCK_TIM1_CLK_EN_Pos);
        } else {
            MDR_RST_CLK->TIM_CLOCK &= ~(Temp << RST_CLK_TIM_CLOCK_TIM1_CLK_EN_Pos);
        }
    }

    /* Handle SSPx. */
    Temp = PER1_CLK & RST_CLK_PER1_CLK_SSP_MASK;
    if (Temp != 0) {
        Temp >>= 11;
        if (NewState != DISABLE) {
            MDR_RST_CLK->SSP_CLOCK |= (Temp << RST_CLK_SSP_CLOCK_SSP1_CLK_EN_Pos);
        } else {
            MDR_RST_CLK->SSP_CLOCK &= ~(Temp << RST_CLK_SSP_CLOCK_SSP1_CLK_EN_Pos);
        }
    }
}

/**
 * @brief  Get the specified peripheral clock work state.
 * @param  PER1_CLK: @ref RST_CLK_PER1_C2_X_TypeDef - specify peripheral clock.
 * @return @ref FunctionalState - the current work state of the specified peripheral clock.
 */
FunctionalState RST_CLK_PER1_C2_GetCmdState(RST_CLK_PER1_C2_X_TypeDef PER1_CLK)
{
    FunctionalState State;

    switch (PER1_CLK) {
        case RST_CLK_PER1_C2_UART1:
            State = (FunctionalState)((MDR_RST_CLK->UART_CLOCK & RST_CLK_UART_CLOCK_UART1_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_UART2:
            State = (FunctionalState)((MDR_RST_CLK->UART_CLOCK & RST_CLK_UART_CLOCK_UART2_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_UART3:
            State = (FunctionalState)((MDR_RST_CLK->UART_CLOCK & RST_CLK_UART_CLOCK_UART3_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_UART4:
            State = (FunctionalState)((MDR_RST_CLK->UART_CLOCK & RST_CLK_UART_CLOCK_UART4_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_TIM1:
            State = (FunctionalState)((MDR_RST_CLK->TIM_CLOCK & RST_CLK_TIM_CLOCK_TIM1_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_TIM2:
            State = (FunctionalState)((MDR_RST_CLK->TIM_CLOCK & RST_CLK_TIM_CLOCK_TIM2_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_TIM3:
            State = (FunctionalState)((MDR_RST_CLK->TIM_CLOCK & RST_CLK_TIM_CLOCK_TIM3_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_TIM4:
            State = (FunctionalState)((MDR_RST_CLK->TIM_CLOCK & RST_CLK_TIM_CLOCK_TIM4_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_SSP1:
            State = (FunctionalState)((MDR_RST_CLK->SSP_CLOCK & RST_CLK_SSP_CLOCK_SSP1_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_SSP2:
            State = (FunctionalState)((MDR_RST_CLK->SSP_CLOCK & RST_CLK_SSP_CLOCK_SSP2_CLK_EN) != 0);
            break;
        case RST_CLK_PER1_C2_SSP3:
            State = (FunctionalState)((MDR_RST_CLK->SSP_CLOCK & RST_CLK_SSP_CLOCK_SSP3_CLK_EN) != 0);
            break;
        default:
            State = DISABLE;
            break;
    }

    return State;
}



/**
 * @brief  Enable or disable BKP keys RAM reset.
 * @param  NewState: @ref FunctionalState - a new state of BKP keys RAM reset.
 * @return None.
 */
void RST_CLK_ResetKeyRAM_Cmd(FunctionalState NewState)
{


    if (NewState != DISABLE) {
        MDR_RST_CLK->PER1_CLOCK |= RST_CLK_PER1_CLOCK_KEY_RESET_PROG;
    } else {
        MDR_RST_CLK->PER1_CLOCK &= ~RST_CLK_PER1_CLOCK_KEY_RESET_PROG;
    }
}

/**
 * @brief  Get BKP keys RAM reset state.
 * @param  None.
 * @return @ref FunctionalState - BKP keys RAM reset state.
 */
FunctionalState RST_CLK_GetResetKeyRAMCmdState(void)
{
    FunctionalState ResetState;

    if ((MDR_RST_CLK->PER1_CLOCK & RST_CLK_PER1_CLOCK_KEY_RESET_PROG) != 0) {
        ResetState = ENABLE;
    } else {
        ResetState = DISABLE;
    }

    return (FunctionalState)ResetState;
}

/**
 * @brief  Configure a division factor for machine timer.
 * @param  SysTimDivValue: Core machine timer division factor.
 *         This parameter can be a value from 2 to 257.
 * @return None.
 */
void RST_CLK_SystemTimer_SetPrescaler(uint16_t SysTimDivValue)
{

    MDR_RST_CLK->DIV_SYS_TIM = SysTimDivValue - 2;
}

/**
 * @brief  Get a division factor for core machine timer.
 * @return SysTimDivValue: Core machine timer division factor (from 2 to 257).
 */
uint16_t RST_CLK_SystemTimer_GetPrescaler(void)
{
    return (uint16_t)(MDR_RST_CLK->DIV_SYS_TIM + 2);
}



/**
 * @brief  Enable or disable clock of peripherals.
 * @param  RST_CLK_PCLK: Specify the peripheral to gate its clock.
 *         This parameter can be any combination of the @ref RST_CLK_PCLK_TypeDef values.
 * @param  NewState: @ref FunctionalState - a new state of the specified peripheral clock.
 * @return None.
 */
void RST_CLK_PCLKCmd(uint32_t RST_CLK_PCLK, FunctionalState NewState)
{


    if (NewState != DISABLE) {
        MDR_RST_CLK->PER2_CLOCK |= RST_CLK_PCLK;
    } else {
        MDR_RST_CLK->PER2_CLOCK &= ~RST_CLK_PCLK;
    }
}

/**
 * @brief  Get the work state of the specified peripheral PLCK clock.
 * @param  RST_CLK_PCLK: @ref RST_CLK_PCLK_TypeDef - specify the peripheral PCLK clock.
 * @return @ref FunctionalState - the current work state of the specified peripheral PLCK clock.
 */
FunctionalState RST_CLK_GetPCLKCmdState(RST_CLK_PCLK_TypeDef RST_CLK_PCLK)
{
    FunctionalState State;


    if ((MDR_RST_CLK->PER2_CLOCK & (uint32_t)RST_CLK_PCLK) == 0) {
        State = DISABLE;
    } else {
        State = ENABLE;
    }

    return State;
}

/**
 * @brief  Check whether a specified RST flag is set or not.
 * @param  RST_CLK_Flag: @ref RST_CLK_Flags_TypeDef - specify a flag to check.
 * @return @ref FlagStatus - the state of clock ready flag (SET or RESET).
 */
FlagStatus RST_CLK_GetFlagStatus(RST_CLK_Flags_TypeDef RST_CLK_Flag)
{
    FlagStatus Status;



    /* Get the flag status on a proper position. */
    if ((MDR_RST_CLK->CLOCK_STATUS & (uint32_t)RST_CLK_Flag) != RESET) {
        Status = SET;
    } else {
        Status = RESET;
    }

    /* Return the flag status. */
    return Status;
}

/**
 * @brief  Put the values of on-chip clock frequencies into RST_CLK_Clocks structure.
 * @param  RST_CLK_Clocks: The pointer to the @ref RST_CLK_Freq_TypeDef structure which will hold
 *         the clock frequencies values.
 * @param  Clocks: Specify on-chip clocks to calculate and put into RST_CLK_Clocks structure.
 *         This parameter can be any combination of @ref RST_CLK_Frequencies_TypeDef values.
 * @return None.
 */
void RST_CLK_GetClocksFreq(RST_CLK_Freq_TypeDef* RST_CLK_Clocks, uint32_t Clocks)
{
    uint32_t CPU_C1_Freq = 0, PLL_Freq, PER1_C1_Freq = 0, HSI_C1_Freq, HSE_C1_Freq;
    uint32_t PER1_C2_Freq = 0;
    uint32_t ADCUI_Freq   = 0;
    uint32_t ADC_Freq     = 0;
    uint32_t HCLK_Freq    = 0;
    uint32_t Temp;

    uint32_t RegPLL, RegCPU, RegPER1, RegRTC;
    uint32_t RegADC = 0;


    /* Read registers. */
    RegPLL  = MDR_RST_CLK->PLL_CONTROL;
    RegCPU  = MDR_RST_CLK->CPU_CLOCK;
    RegPER1 = MDR_RST_CLK->PER1_CLOCK;
    RegRTC  = MDR_RST_CLK->RTC_CLOCK;

    /* Determine common sources frequencies. */

    /* Determine the CPU_C1 frequency. */
    switch (RegCPU & RST_CLK_CPU_CLOCK_CPU_C1_SEL_Msk) {
        case RST_CLK_CPU_C1_CLK_SRC_HSI:
            CPU_C1_Freq = HSI_FREQUENCY_Hz;
            break;
        case RST_CLK_CPU_C1_CLK_SRC_HSI_DIV_2:
            CPU_C1_Freq = HSI_FREQUENCY_Hz / 2;
            break;
        case RST_CLK_CPU_C1_CLK_SRC_HSE:
            CPU_C1_Freq = HSE_FREQUENCY_Hz;
            break;
        case RST_CLK_CPU_C1_CLK_SRC_HSE_DIV_2:
            CPU_C1_Freq = HSE_FREQUENCY_Hz / 2;
            break;
    }

    /* Determine the CPU PLL frequency. */
    Temp = ((RegPLL & RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Msk) >> RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos) + 1;
    if ((RegPLL & RST_CLK_PLL_CONTROL_PLL_CPU_SEL) != 0) {
        PLL_Freq = CPU_C1_Freq * Temp;
    } else {
        PLL_Freq = HSE_FREQUENCY_Hz * Temp;
    }

    /* Determine the PER_C1 frequency. */
    switch (RegPER1 & RST_CLK_PER1_CLOCK_PER1_C1_SEL_Msk) {
        case RST_CLK_PER1_C1_CLK_SRC_LSI:
            PER1_C1_Freq = LSI_FREQUENCY_Hz;
            break;
        case RST_CLK_PER1_C1_CLK_SRC_LSI_DIV_2:
            PER1_C1_Freq = LSI_FREQUENCY_Hz / 2;
            break;
        case RST_CLK_PER1_C1_CLK_SRC_LSE:
            PER1_C1_Freq = LSE_FREQUENCY_Hz;
            break;
        case RST_CLK_PER1_C1_CLK_SRC_LSE_DIV_2:
            PER1_C1_Freq = LSE_FREQUENCY_Hz / 2;
            break;
    }

    /* Compute the RTCHSE_CLK frequency. */
    if ((RegRTC & ((uint32_t)0x8)) == 0) {
        HSE_C1_Freq = HSE_FREQUENCY_Hz;
    } else {
        HSE_C1_Freq = HSE_FREQUENCY_Hz >> ((RegRTC & ((uint32_t)0x7)) + 1);
    }
    if ((Clocks & RST_CLK_CLOCK_FREQ_RTCHSE) != 0) {
        RST_CLK_Clocks->RTCHSE_Frequency = HSE_C1_Freq;
    }

    /* Compute the RTCHSI_CLK frequency. */
    Temp = RegRTC >> RST_CLK_RTC_CLOCK_HSI_SEL_Pos;
    if ((Temp & ((uint32_t)0x8)) == 0) {
        HSI_C1_Freq = HSI_FREQUENCY_Hz;
    } else {
        HSI_C1_Freq = HSI_FREQUENCY_Hz >> ((Temp & ((uint32_t)0x7)) + 1);
    }
    if ((Clocks & RST_CLK_CLOCK_FREQ_RTCHSI) != 0) {
        RST_CLK_Clocks->RTCHSI_Frequency = HSI_C1_Freq;
    }

    /* Compute necessary frequencies. */

#if defined(USE_MDR32F02_REV_1X)
    if ((Clocks & RST_CLK_CLOCK_FREQ_CPU_CLK) != 0) {
#else
    if ((Clocks & (RST_CLK_CLOCK_FREQ_CPU_CLK | RST_CLK_CLOCK_FREQ_MACHINE_TIMER)) != 0) {
#endif
        switch (RegCPU & RST_CLK_CPU_CLOCK_HCLK_SEL_Msk) {
            case RST_CLK_CPU_CLOCK_HCLK_SEL_HSI:
                HCLK_Freq = HSI_FREQUENCY_Hz;
                break;
            case RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3:
                /* Determine the CPU_C2 frequency. */
                if ((RegCPU & RST_CLK_CPU_CLOCK_CPU_C2_SEL) != 0) {
                    HCLK_Freq = PLL_Freq;
                } else {
                    HCLK_Freq = CPU_C1_Freq;
                }

                /* Determine the CPU_C3 frequency. */
                Temp = ((RegCPU & RST_CLK_CPU_CLOCK_CPU_C3_SEL_Msk) >> RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos);
                if ((Temp & ((uint32_t)0x8)) != 0x00) {
                    HCLK_Freq = HCLK_Freq >> ((Temp & ((uint32_t)0x7)) + 1);
                }
                break;
            case RST_CLK_CPU_CLOCK_HCLK_SEL_LSE:
                HCLK_Freq = LSE_FREQUENCY_Hz;
                break;
            case RST_CLK_CPU_CLOCK_HCLK_SEL_LSI:
                HCLK_Freq = LSI_FREQUENCY_Hz;
                break;
        }
        if ((Clocks & RST_CLK_CLOCK_FREQ_CPU_CLK) != 0) {
            RST_CLK_Clocks->CPU_CLK_Frequency = HCLK_Freq;
        }
    }

    if ((Clocks & RST_CLK_CLOCK_FREQ_MACHINE_TIMER) != 0) {
#if defined(USE_MDR32F02_REV_1X)
        RST_CLK_Clocks->MachineTimer_Frequency = LSE_FREQUENCY_Hz;
#else
        Temp                                   = (MDR_RST_CLK->DIV_SYS_TIM & RST_CLK_DIV_SYS_TIM_DIV_SYS_TIM_Msk) + 2;
        RST_CLK_Clocks->MachineTimer_Frequency = HCLK_Freq / Temp;
#endif
    }

    if ((Clocks & (RST_CLK_CLOCK_FREQ_UART1 | RST_CLK_CLOCK_FREQ_UART2 | RST_CLK_CLOCK_FREQ_UART3 | RST_CLK_CLOCK_FREQ_UART4 |
                   RST_CLK_CLOCK_FREQ_TIMER1 | RST_CLK_CLOCK_FREQ_TIMER2 | RST_CLK_CLOCK_FREQ_TIMER3 | RST_CLK_CLOCK_FREQ_TIMER4 |
                   RST_CLK_CLOCK_FREQ_SSP1 | RST_CLK_CLOCK_FREQ_SSP2 | RST_CLK_CLOCK_FREQ_SSP3)) != 0) {
        /* Determine clock frequency PER1_C2. */
        switch (RegPER1 & RST_CLK_PER1_CLOCK_PER1_C2_SEL_Msk) {
            case RST_CLK_PER1_C2_CLK_SRC_CPU_C1:
                PER1_C2_Freq = CPU_C1_Freq;
                break;
            case RST_CLK_PER1_C2_CLK_SRC_PER1_C1:
                PER1_C2_Freq = PER1_C1_Freq;
                break;
            case RST_CLK_PER1_C2_CLK_SRC_PLLCPU:
                PER1_C2_Freq = PLL_Freq;
                break;
            case RST_CLK_PER1_C2_CLK_SRC_HSI_C1_CLK:
                PER1_C2_Freq = HSI_C1_Freq;
                break;
        }
        if ((Clocks & (RST_CLK_CLOCK_FREQ_UART1 | RST_CLK_CLOCK_FREQ_UART2 | RST_CLK_CLOCK_FREQ_UART3 | RST_CLK_CLOCK_FREQ_UART4)) != 0) {
            Temp = MDR_RST_CLK->UART_CLOCK;
            if ((Clocks & RST_CLK_CLOCK_FREQ_UART1) != 0) {
                RST_CLK_Clocks->UART_Frequency[0] = PER1_C2_Freq >> ((Temp & RST_CLK_UART_CLOCK_UART1_BRG_Msk) >> RST_CLK_UART_CLOCK_UART1_BRG_Pos);
            }
            if ((Clocks & RST_CLK_CLOCK_FREQ_UART2) != 0) {
                RST_CLK_Clocks->UART_Frequency[1] = PER1_C2_Freq >> ((Temp & RST_CLK_UART_CLOCK_UART2_BRG_Msk) >> RST_CLK_UART_CLOCK_UART2_BRG_Pos);
            }
            if ((Clocks & RST_CLK_CLOCK_FREQ_UART3) != 0) {
                RST_CLK_Clocks->UART_Frequency[2] = PER1_C2_Freq >> ((Temp & RST_CLK_UART_CLOCK_UART3_BRG_Msk) >> RST_CLK_UART_CLOCK_UART3_BRG_Pos);
            }
            if ((Clocks & RST_CLK_CLOCK_FREQ_UART4) != 0) {
                RST_CLK_Clocks->UART_Frequency[3] = PER1_C2_Freq >> ((Temp & RST_CLK_UART_CLOCK_UART4_BRG_Msk) >> RST_CLK_UART_CLOCK_UART4_BRG_Pos);
            }
        }
        if ((Clocks & (RST_CLK_CLOCK_FREQ_TIMER1 | RST_CLK_CLOCK_FREQ_TIMER2 | RST_CLK_CLOCK_FREQ_TIMER3 | RST_CLK_CLOCK_FREQ_TIMER4)) != 0) {
            Temp = MDR_RST_CLK->TIM_CLOCK;
            if ((Clocks & RST_CLK_CLOCK_FREQ_TIMER1) != 0) {
                RST_CLK_Clocks->TIMER_Frequency[0] = PER1_C2_Freq >> ((Temp & RST_CLK_TIM_CLOCK_TIM1_BRG_Msk) >> RST_CLK_TIM_CLOCK_TIM1_BRG_Pos);
            }
            if ((Clocks & RST_CLK_CLOCK_FREQ_TIMER2) != 0) {
                RST_CLK_Clocks->TIMER_Frequency[1] = PER1_C2_Freq >> ((Temp & RST_CLK_TIM_CLOCK_TIM2_BRG_Msk) >> RST_CLK_TIM_CLOCK_TIM2_BRG_Pos);
            }
            if ((Clocks & RST_CLK_CLOCK_FREQ_TIMER3) != 0) {
                RST_CLK_Clocks->TIMER_Frequency[2] = PER1_C2_Freq >> ((Temp & RST_CLK_TIM_CLOCK_TIM3_BRG_Msk) >> RST_CLK_TIM_CLOCK_TIM3_BRG_Pos);
            }
            if ((Clocks & RST_CLK_CLOCK_FREQ_TIMER4) != 0) {
                RST_CLK_Clocks->TIMER_Frequency[3] = PER1_C2_Freq >> ((Temp & RST_CLK_TIM_CLOCK_TIM4_BRG_Msk) >> RST_CLK_TIM_CLOCK_TIM4_BRG_Pos);
            }
        }
        if ((Clocks & (RST_CLK_CLOCK_FREQ_SSP1 | RST_CLK_CLOCK_FREQ_SSP2 | RST_CLK_CLOCK_FREQ_SSP3)) != 0) {
            Temp = MDR_RST_CLK->SSP_CLOCK;
            if ((Clocks & RST_CLK_CLOCK_FREQ_SSP1) != 0) {
                RST_CLK_Clocks->SSP_Frequency[0] = PER1_C2_Freq >> ((Temp & RST_CLK_SSP_CLOCK_SSP1_BRG_Msk) >> RST_CLK_SSP_CLOCK_SSP1_BRG_Pos);
            }
            if ((Clocks & RST_CLK_CLOCK_FREQ_SSP2) != 0) {
                RST_CLK_Clocks->SSP_Frequency[1] = PER1_C2_Freq >> ((Temp & RST_CLK_SSP_CLOCK_SSP2_BRG_Msk) >> RST_CLK_SSP_CLOCK_SSP2_BRG_Pos);
            }
            if ((Clocks & RST_CLK_CLOCK_FREQ_SSP3) != 0) {
                RST_CLK_Clocks->SSP_Frequency[2] = PER1_C2_Freq >> ((Temp & RST_CLK_SSP_CLOCK_SSP3_BRG_Msk) >> RST_CLK_SSP_CLOCK_SSP3_BRG_Pos);
            }
        }
    }

    if ((Clocks & RST_CLK_CLOCK_FREQ_WWDG) != 0) {
        switch (RegPER1 & RST_CLK_PER1_CLOCK_WDG_C2_SEL_Msk) {
            case RST_CLK_WWDG_C2_CLK_SRC_CPU_C1:
                Temp = CPU_C1_Freq;
                break;
            case RST_CLK_WWDG_C2_CLK_SRC_WWDG_C1:
                switch (RegPER1 & RST_CLK_PER1_CLOCK_WDG_C1_SEL_Msk) {
                    case RST_CLK_WWDG_C1_CLK_SRC_LSI:
                        Temp = LSI_FREQUENCY_Hz;
                        break;
                    case RST_CLK_WWDG_C1_CLK_SRC_LSI_DIV_2:
                        Temp = LSI_FREQUENCY_Hz / 2;
                        break;
                    case RST_CLK_WWDG_C1_CLK_SRC_LSE:
                        Temp = LSE_FREQUENCY_Hz;
                        break;
                    case RST_CLK_WWDG_C1_CLK_SRC_LSE_DIV_2:
                        Temp = LSE_FREQUENCY_Hz / 2;
                        break;
                }
                break;
            case RST_CLK_WWDG_C2_CLK_SRC_PLLCPU:
                Temp = PLL_Freq;
                break;
            case RST_CLK_WWDG_C2_CLK_SRC_HSI_C1_CLK:
                Temp = HSI_C1_Freq;
                break;
        }
        RST_CLK_Clocks->WWDG_Frequency = Temp >> ((RegPER1 & RST_CLK_PER1_CLOCK_WDG_BRG_Msk) >> RST_CLK_PER1_CLOCK_WDG_BRG_Pos);
    }

    if ((Clocks & (RST_CLK_CLOCK_FREQ_ADC | RST_CLK_CLOCK_FREQ_ADCUI)) != 0) {
        RegADC = MDR_RST_CLK->ADC_CLOCK;
        if ((Clocks & RST_CLK_CLOCK_FREQ_ADC) != 0) {
            switch (RegADC & RST_CLK_ADC_CLOCK_ADC_C1_SEL_Msk) {
                case RST_CLK_ADC_CLK_SRC_CPU_C1:
                    ADC_Freq = CPU_C1_Freq;
                    break;
                case RST_CLK_ADC_CLK_SRC_PER1_C1:
                    ADC_Freq = PER1_C1_Freq;
                    break;
                case RST_CLK_ADC_CLK_SRC_PLLCPU:
                    ADC_Freq = PLL_Freq;
                    break;
                case RST_CLK_ADC_CLK_SRC_HSE_C1_CLK:
                    ADC_Freq = HSE_C1_Freq;
                    break;
            }
            Temp = RegADC >> RST_CLK_ADC_CLOCK_ADC_C3_SEL_Pos;
            if ((Temp & ((uint32_t)0x8)) != 0) {
                ADC_Freq = ADC_Freq >> ((Temp & (uint32_t)0x7) + 1);
            }

            RST_CLK_Clocks->ADC_Frequency = ADC_Freq;
        }
    }

    if ((Clocks & RST_CLK_CLOCK_FREQ_ADCUI) != 0) {
        switch (RegADC & RST_CLK_ADC_CLOCK_ADCUI_C1_SEL_Msk) {
            case RST_CLK_ADCUI_CLK_SRC_CPU_C1:
                ADCUI_Freq = CPU_C1_Freq;
                break;
            case RST_CLK_ADCUI_CLK_SRC_PER1_C1:
                ADCUI_Freq = PER1_C1_Freq;
                break;
            case RST_CLK_ADCUI_CLK_SRC_PLLCPU:
                ADCUI_Freq = PLL_Freq;
                break;
            case RST_CLK_ADCUI_CLK_SRC_HSE_C1_CLK:
                ADCUI_Freq = HSE_C1_Freq;
                break;
        }
        Temp = RegADC >> RST_CLK_ADC_CLOCK_ADCUI_C3_SEL_Pos;
        if ((Temp & ((uint32_t)0x8)) != 0) {
            ADCUI_Freq = ADCUI_Freq >> ((Temp & ((uint32_t)0x7)) + 1);
        }

        RST_CLK_Clocks->ADCUI_Frequency = ADCUI_Freq;
    }
}

