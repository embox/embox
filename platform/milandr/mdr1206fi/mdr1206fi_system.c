/**
 ******************************************************************************
 * @file    system_MDR32VF0xI.c
 * @author  Milandr Application Team
 * @version V0.2.0
 * @date    10/04/2025
 * @brief   Device Peripheral Access Layer System Source File for MDR32VF0xI.
 ******************************************************************************
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
 ******************************************************************************
 */

#include <stdint.h>

#include "rst_clk.h"

#include "mdr1206fi.h"

uint32_t SystemCoreClock = HSI_FREQUENCY_Hz / 16;


/** @} */ /* End of the group MDR32VF0xI_System_Private_Functions_Declarations */

/** @defgroup MDR32VF0xI_System_Exported_Functions MDR32VF0xI System Exported Functions
 * @{
 */

/**
 * @brief  Update SystemCoreClock according to clock register values.
 * @param  None.
 * @return None.
 */
void SystemCoreClockUpdate(void)
{
    uint32_t CPU_C1_Freq = 0, CPU_C2_Freq = 0, CPU_C3_Freq = 0;
    uint32_t PLL_Mult = 0;
    uint32_t Temp1 = 0, Temp2 = 0;

    /* Compute CPU_CLK frequency. */

    Temp1 = MDR_RST_CLK->CPU_CLOCK;

    /* Select CPU_CLK from HSI, CPU_C3, LSE, LSI cases. */
    switch (Temp1 & RST_CLK_CPU_CLOCK_HCLK_SEL_Msk) {
        case RST_CLK_CPU_CLOCK_HCLK_SEL_HSI:
            SystemCoreClock = HSI_FREQUENCY_Hz;
            break;

        case RST_CLK_CPU_CLOCK_HCLK_SEL_LSE:
            SystemCoreClock = LSE_FREQUENCY_Hz;
            break;

        case RST_CLK_CPU_CLOCK_HCLK_SEL_LSI:
            SystemCoreClock = LSI_FREQUENCY_Hz;
            break;

        case RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3:
            /* Determine CPU_C1 frequency. */
            switch (Temp1 & RST_CLK_CPU_CLOCK_CPU_C1_SEL_Msk) {
                case RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSI:
                    CPU_C1_Freq = HSI_FREQUENCY_Hz;
                    break;
                case RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSI_DIV_2:
                    CPU_C1_Freq = HSI_FREQUENCY_Hz / 2;
                    break;
                case RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE:
                    CPU_C1_Freq = HSE_FREQUENCY_Hz;
                    break;
                case RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE_DIV_2:
                    CPU_C1_Freq = HSE_FREQUENCY_Hz / 2;
                    break;
            }

            /* Determine CPU_C2 frequency. */
            if ((Temp1 & RST_CLK_CPU_CLOCK_CPU_C2_SEL) != 0) {
                /* Determine CPU PLL output frequency. */
                Temp2    = MDR_RST_CLK->PLL_CONTROL;
                PLL_Mult = ((Temp2 & RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Msk) >> RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos) + 1;

                /* Determine CPU_PLL source. */
                if ((Temp2 & RST_CLK_PLL_CONTROL_PLL_CPU_SEL) != 0) {
                    CPU_C2_Freq = CPU_C1_Freq * PLL_Mult;
                } else {
                    CPU_C2_Freq = HSE_FREQUENCY_Hz * PLL_Mult;
                }
            } else {
                CPU_C2_Freq = CPU_C1_Freq;
            }

            /* Determine CPU_C3 frequency. */
            Temp1 = ((Temp1 & RST_CLK_CPU_CLOCK_CPU_C3_SEL_Msk) >> RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos);
            if ((Temp1 & 0x8U) == 0x00) {
                CPU_C3_Freq = CPU_C2_Freq;
            } else {
                CPU_C3_Freq = CPU_C2_Freq >> ((Temp1 & 0x7U) + 1);
            }
            SystemCoreClock = CPU_C3_Freq;
            break;
    }
}
