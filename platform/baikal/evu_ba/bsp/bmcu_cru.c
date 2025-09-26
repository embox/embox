/**
 * *****************************************************************************
 *  @file       bmcu_cru.c
 *  @author     Baikal electronics SDK team
 *  @brief      Control Registers Unit (CRU) module driver
 *  @version    1.3
 *  @date       2025-06-30
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#include "bmcu_cru.h"

static uint32_t CRU_GetCCLKClockFreq(void);
static uint32_t CRU_GetPCLK0ClockFreq(void);
static uint32_t CRU_GetPCLK1ClockFreq(void);
static uint32_t CRU_GetPCLK2ClockFreq(void);
static uint32_t CRU_GetHCLKClockFreq(void);
static uint32_t CRU_GetTCLKClockFreq(void);
static uint32_t CRU_GetCANx2CLKClockFreq(void);
static uint32_t CRU_CalcPLLClkFreq(uint32_t FRef);

/**
 * @brief Resets CRU clocks configuration to the default state.
 * @note The default state of the clocks configuration is as follows:
 *         - C0 is used as AXI, APB0, APB1, APB2, AHB, TRACE, CAN clocks source
 *         - PLL is OFF
 *         - AXI, APB0, APB1, APB2, AHB, TRACE, CAN prescalers are set to 1 and disabled
 * @note This function doesn't modify the configuration of the peripheral clocks.
 * @returns SUCCESS if completed successfully, otherwise ERROR.
 */
ErrorStatus CRU_DeInit(void)
{
    /* Enable RC generator */
    CRU_RCGen_Enable();

    /* Set boot clock source to STRAP */
    CRU_SetBootClkSource(CRU_BOOT_CLK_STRAP);

    /* Disable CLKI clock monitoring system */
    CRU_ClkMon_Disable();

    /* Configure C0 as a source of all clocks */
    CRU_PLL_SetSource(CRU_PLL_SRC_C0);
    CRU_SetCCLKSource(CRU_CLK_SRC_C0);
    CRU_SetPCLK0Source(CRU_CLK_SRC_C0);
    CRU_SetPCLK1Source(CRU_CLK_SRC_C0);
    CRU_SetPCLK2Source(CRU_CLK_SRC_C0);
    CRU_SetHCLKSource(CRU_CLK_SRC_C0);
    CRU_SetTCLKSource(CRU_CLK_SRC_C0);
    CRU_SetCANx2CLKSource(CRU_CLK_SRC_C0);

    /* Disable PLL */
    CRU_PLL_Disable();

    /* Set all clock prescalers to 1 and disable */
    CRU_SetCCLKPrescaler(CRU_CLK_DIV_1);
    CRU_CCLKPrescaler_Disable();
    CRU_SetPCLK0Prescaler(CRU_CLK_DIV_1);
    CRU_PCLK0Prescaler_Disable();
    CRU_SetPCLK1Prescaler(CRU_CLK_DIV_1);
    CRU_PCLK1Prescaler_Disable();
    CRU_SetPCLK2Prescaler(CRU_CLK_DIV_1);
    CRU_PCLK2Prescaler_Disable();
    CRU_SetHCLKPrescaler(CRU_CLK_DIV_1);
    CRU_HCLKPrescaler_Disable();
    CRU_SetTCLKPrescaler(CRU_CLK_DIV_1);
    CRU_TCLKPrescaler_Disable();
    CRU_SetCANx2CLKPrescaler(CRU_CLK_DIV_1);
    CRU_CANx2CLKPrescaler_Disable();

    return SUCCESS;
}

/**
 * @brief Updates clocks frequency values.
 * @note Each time clocks change, this function must be called to update
 *       CRU_Clocks structure fields. Otherwise, any configuration based
 *       on this function will be incorrect.
 * @param CRU_Clocks The clocks frequency structure.
 */
void CRU_GetSystemClocksFreq(CRU_ClocksTypeDef * CRU_Clocks)
{
    /* Get CCLK frequency */
    CRU_Clocks->CCLK_Frequency = CRU_GetCCLKClockFreq();

    /* Get PCLK0 frequency */
    CRU_Clocks->PCLK0_Frequency = CRU_GetPCLK0ClockFreq();

    /* Get PCLK1 frequency */
    CRU_Clocks->PCLK1_Frequency = CRU_GetPCLK1ClockFreq();

    /* Get PCLK2 frequency */
    CRU_Clocks->PCLK2_Frequency = CRU_GetPCLK2ClockFreq();

    /* Get HCLK frequency */
    CRU_Clocks->HCLK_Frequency = CRU_GetHCLKClockFreq();

    /* Get TCLK frequency */
    CRU_Clocks->TCLK_Frequency = CRU_GetTCLKClockFreq();

    /* Get CANx2CLK frequency */
    CRU_Clocks->CANx2CLK_Frequency = CRU_GetCANx2CLKClockFreq();
}

/**
 * @brief Returns CCLK clock frequency.
 * @returns CCLK clock frequency (Hz).
 */
uint32_t CRU_GetCCLKClockFreq(void)
{
    uint32_t frequency = 0UL;

    switch (CRU_GetCCLKSource())
    {
        case CRU_CLK_SRC_C0:
        {
            /* C0 used as AXI domain clock (CCLK) source */
            frequency = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        case CRU_CLK_SRC_PLL:
        {
            /* PLL used as AXI domain clock (CCLK) source */
            
            if (CRU_CCLKPrescaler_IsEnabled())
            {
                /* CCLK prescaler is enabled */

                uint32_t f_pll_in;
                if (CRU_PLL_GetSource() == CRU_PLL_SRC_C0)
                {
                    /* PLL input clock is C0 */
                    f_pll_in = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }
                else
                {
                    /* PLL input clock is C1 */
                    f_pll_in = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }

                /* Calculate PLL output frequency */
                uint32_t f_pll_out = CRU_CalcPLLClkFreq(f_pll_in);

                /* Calculate prescaler output frequency */
                uint32_t prescaler = (uint32_t)CRU_GetCCLKPrescaler();

                if (prescaler & 0x1UL)
                {
                    frequency = (uint32_t)(((uint64_t)f_pll_out) * 10U / (((prescaler >> 1) + 1U) * 10U + 5U));
                }
                else
                {
                    frequency = f_pll_out / ((prescaler >> 1) + 1U);
                }
            }
        }
        break;

        case CRU_CLK_SRC_C1:
        {
            /* C1 used as AXI domain clock (CCLK) source */
            frequency = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        default:
        {
            /* Should never get here */
        }
        break;
    }

    return frequency;
}

/**
 * @brief Returns PCLK0 clock frequency.
 * @returns PCLK0 clock frequency (Hz).
 */
uint32_t CRU_GetPCLK0ClockFreq(void)
{
    uint32_t frequency = 0UL;

    switch (CRU_GetPCLK0Source())
    {
        case CRU_CLK_SRC_C0:
        {
            /* C0 clock selected as PCLK0 clock source */
            frequency = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        case CRU_CLK_SRC_PLL:
        {
            /* PLL clock selected as PCLK0 clock source */
            
            if (CRU_PCLK0Prescaler_IsEnabled())
            {
                /* PCLK0 prescaler is enabled */

                uint32_t f_pll_in;
                if (CRU_PLL_GetSource() == CRU_PLL_SRC_C0)
                {
                    /* PLL input clock is C0 */
                    f_pll_in = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }
                else
                {
                    /* PLL input clock is C1 */
                    f_pll_in = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }

                /* Calculate PLL output frequency */
                uint32_t f_pll_out = CRU_CalcPLLClkFreq(f_pll_in);

                /* Calculate prescaler output frequency */
                uint32_t prescaler = (uint32_t)CRU_GetPCLK0Prescaler();

                if (prescaler & 0x1UL)
                {
                    frequency = (uint32_t)(((uint64_t)f_pll_out) * 10U / (((prescaler >> 1) + 1U) * 10U + 5U));
                }
                else
                {
                    frequency = f_pll_out / ((prescaler >> 1) + 1U);
                }
            }
        }
        break;

        case CRU_CLK_SRC_C1:
        {
            /* C1 clock selected as PCLK0 clock source */
            frequency = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        default:
        {
            /* Should never get here */
        }
        break;
    }

    return frequency;
}

/**
 * @brief Returns PCLK1 clock frequency.
 * @returns PCLK1 clock frequency (Hz).
 */
uint32_t CRU_GetPCLK1ClockFreq(void)
{
    uint32_t frequency = 0UL;

    switch (CRU_GetPCLK1Source())
    {
        case CRU_CLK_SRC_C0:
        {
            /* C0 clock selected as PCLK1 clock source */
            frequency = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        case CRU_CLK_SRC_PLL:
        {
            /* PLL clock selected as PCLK1 clock source */
            
            if (CRU_PCLK1Prescaler_IsEnabled())
            {
                /* PCLK1 prescaler is enabled */

                uint32_t f_pll_in;
                if (CRU_PLL_GetSource() == CRU_PLL_SRC_C0)
                {
                    /* PLL input clock is C0 */
                    f_pll_in = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }
                else
                {
                    /* PLL input clock is C1 */
                    f_pll_in = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }

                /* Calculate PLL output frequency */
                uint32_t f_pll_out = CRU_CalcPLLClkFreq(f_pll_in);

                /* Calculate prescaler output frequency */
                uint32_t prescaler = (uint32_t)CRU_GetPCLK1Prescaler();

                if (prescaler & 0x1UL)
                {
                    frequency = (uint32_t)(((uint64_t)f_pll_out) * 10U / (((prescaler >> 1) + 1U) * 10U + 5U));
                }
                else
                {
                    frequency = f_pll_out / ((prescaler >> 1) + 1U);
                }
            }
        }
        break;

        case CRU_CLK_SRC_C1:
        {
            /* C1 clock selected as PCLK1 clock source */
            frequency = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        default:
        {
            /* Should never get here */
        }
        break;
    }

    return frequency;
}

/**
 * @brief Returns PCLK2 clock frequency.
 * @returns PCLK2 clock frequency (Hz).
 */
uint32_t CRU_GetPCLK2ClockFreq(void)
{
    uint32_t frequency = 0UL;

    switch (CRU_GetPCLK2Source())
    {
        case CRU_CLK_SRC_C0:
        {
            /* C0 clock selected as PCLK2 clock source */
            frequency = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        case CRU_CLK_SRC_PLL:
        {
            /* PLL clock selected as PCLK2 clock source */
            
            if (CRU_PCLK2Prescaler_IsEnabled())
            {
                /* PCLK2 prescaler is enabled */

                uint32_t f_pll_in;
                if (CRU_PLL_GetSource() == CRU_PLL_SRC_C0)
                {
                    /* PLL input clock is C0 */
                    f_pll_in = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }
                else
                {
                    /* PLL input clock is C1 */
                    f_pll_in = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }

                /* Calculate PLL output frequency */
                uint32_t f_pll_out = CRU_CalcPLLClkFreq(f_pll_in);

                /* Calculate prescaler output frequency */
                uint32_t prescaler = (uint32_t)CRU_GetPCLK2Prescaler();

                if (prescaler & 0x1UL)
                {
                    frequency = (uint32_t)(((uint64_t)f_pll_out) * 10U / (((prescaler >> 1) + 1U) * 10U + 5U));
                }
                else
                {
                    frequency = f_pll_out / ((prescaler >> 1) + 1U);
                }
            }
        }
        break;

        case CRU_CLK_SRC_C1:
        {
            /* C1 clock selected as PCLK2 clock source */
            frequency = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        default:
        {
            /* Should never get here */
        }
        break;
    }

    return frequency;
}

/**
 * @brief Returns HCLK clock frequency.
 * @returns HCLK clock frequency (Hz).
 */
uint32_t CRU_GetHCLKClockFreq(void)
{
    uint32_t frequency = 0UL;

    switch (CRU_GetHCLKSource())
    {
        case CRU_CLK_SRC_C0:
        {
            /* C0 clock selected as HCLK clock source */
            frequency = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        case CRU_CLK_SRC_PLL:
        {
            /* PLL clock selected as HCLK clock source */
            
            if (CRU_HCLKPrescaler_IsEnabled())
            {
                /* HCLK prescaler is enabled */

                uint32_t f_pll_in;
                if (CRU_PLL_GetSource() == CRU_PLL_SRC_C0)
                {
                    /* PLL input clock is C0 */
                    f_pll_in = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }
                else
                {
                    /* PLL input clock is C1 */
                    f_pll_in = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }

                /* Calculate PLL output frequency */
                uint32_t f_pll_out = CRU_CalcPLLClkFreq(f_pll_in);

                /* Calculate prescaler output frequency */
                uint32_t prescaler = (uint32_t)CRU_GetHCLKPrescaler();

                if (prescaler & 0x1UL)
                {
                    frequency = (uint32_t)(((uint64_t)f_pll_out) * 10U / (((prescaler >> 1) + 1U) * 10U + 5U));
                }
                else
                {
                    frequency = f_pll_out / ((prescaler >> 1) + 1U);
                }
            }
        }
        break;

        case CRU_CLK_SRC_C1:
        {
            /* C1 clock selected as HCLK clock source */
            frequency = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        default:
        {
            /* Should never get here */
        }
        break;
    }

    return frequency;
}

/**
 * @brief Returns TCLK clock frequency.
 * @returns TCLK clock frequency (Hz).
 */
uint32_t CRU_GetTCLKClockFreq(void)
{
    uint32_t frequency = 0UL;

    switch (CRU_GetTCLKSource())
    {
        case CRU_CLK_SRC_C0:
        {
            /* C0 clock selected as TCLK clock source */
            frequency = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        case CRU_CLK_SRC_PLL:
        {
            /* PLL clock selected as TCLK clock source */
            
            if (CRU_TCLKPrescaler_IsEnabled())
            {
                /* TCLK prescaler is enabled */

                uint32_t f_pll_in;
                if (CRU_PLL_GetSource() == CRU_PLL_SRC_C0)
                {
                    /* PLL input clock is C0 */
                    f_pll_in = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }
                else
                {
                    /* PLL input clock is C1 */
                    f_pll_in = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }

                /* Calculate PLL output frequency */
                uint32_t f_pll_out = CRU_CalcPLLClkFreq(f_pll_in);

                /* Calculate prescaler output frequency */
                uint32_t prescaler = (uint32_t)CRU_GetTCLKPrescaler();

                if (prescaler & 0x1UL)
                {
                    frequency = (uint32_t)(((uint64_t)f_pll_out) * 10U / (((prescaler >> 1) + 1U) * 10U + 5U));
                }
                else
                {
                    frequency = f_pll_out / ((prescaler >> 1) + 1U);
                }
            }
        }
        break;

        case CRU_CLK_SRC_C1:
        {
            /* C1 clock selected as TCLK clock source */
            frequency = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        default:
        {
            /* Should never get here */
        }
        break;
    }

    return frequency;
}

/**
 * @brief Returns CANx2CLK clock frequency.
 * @returns CANx2CLK clock frequency (Hz).
 */
uint32_t CRU_GetCANx2CLKClockFreq(void)
{
    uint32_t frequency = 0UL;

    switch (CRU_GetCANx2CLKSource())
    {
        case CRU_CLK_SRC_C0:
        {
            /* C0 clock selected as CANx2CLK clock source */
            frequency = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        case CRU_CLK_SRC_PLL:
        {
            /* PLL clock selected as CANx2CLK clock source */
            
            if (CRU_CANx2CLKPrescaler_IsEnabled())
            {
                /* CANx2CLK prescaler is enabled */

                uint32_t f_pll_in;
                if (CRU_PLL_GetSource() == CRU_PLL_SRC_C0)
                {
                    /* PLL input clock is C0 */
                    f_pll_in = (CRU_GetC0Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }
                else
                {
                    /* PLL input clock is C1 */
                    f_pll_in = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
                }

                /* Calculate PLL output frequency */
                uint32_t f_pll_out = CRU_CalcPLLClkFreq(f_pll_in);

                /* Calculate prescaler output frequency */
                uint32_t prescaler = (uint32_t)CRU_GetCANx2CLKPrescaler();

                if (prescaler & 0x1UL)
                {
                    frequency = (uint32_t)(((uint64_t)f_pll_out) * 10U / (((prescaler >> 1) + 1U) * 10U + 5U));
                }
                else
                {
                    frequency = f_pll_out / ((prescaler >> 1) + 1U);
                }
            }
        }
        break;

        case CRU_CLK_SRC_C1:
        {
            /* C1 clock selected as CANx2CLK clock source */
            frequency = (CRU_GetC1Source() == CRU_MCU_CLK_CLKI) ? (HSE_VALUE) : (HSI_VALUE);
        }
        break;

        default:
        {
            /* Should never get here */
        }
        break;
    }

    return frequency;
}

/**
 * @brief Calculates PLL output frequency.
 * @param FRef PLL input frequency (Hz).
 * @returns PLL output frequency (Hz).
 */
uint32_t CRU_CalcPLLClkFreq(uint32_t FRef)
{
    uint32_t pll_cfg = CRU->PLLSET;

    return (FRef * (((pll_cfg & CRU_PLLSET_CLKF) >> CRU_PLLSET_CLKF_Pos) + 1U) /
            (((pll_cfg & CRU_PLLSET_CLKR) >> CRU_PLLSET_CLKR_Pos) + 1U) /
            (((pll_cfg & CRU_PLLSET_CLKOD) >> CRU_PLLSET_CLKOD_Pos) + 1U));
}
