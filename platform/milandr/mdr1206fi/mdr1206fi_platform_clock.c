#include "rst_clk.h"

#include "mdr1206fi.h"

extern void SystemCoreClockUpdate(void);
void ClockConfig()
{
    int ErrorCode;
    static const RST_CLK_HCLK_InitTypeDef Clk = {
        .RST_CLK_CPU_C1_Source      = RST_CLK_CPU_C1_CLK_SRC_HSE,
        .RST_CLK_PLLCPU_ClockSource = RST_CLK_PLLCPU_CLK_SRC_HSE,
        .RST_CLK_PLLCPU_Multiplier  = RST_CLK_PLLCPU_MUL_4,
        .RST_CLK_CPU_C2_ClockSource = RST_CLK_CPU_C2_CLK_SRC_PLLCPU,
        .RST_CLK_CPU_C3_Prescaler   = RST_CLK_CPU_C3_PRESCALER_DIV_1,
        .RST_CLK_HCLK_ClockSource   = RST_CLK_CPU_HCLK_CLK_SRC_CPU_C3,
    };

    RST_CLK_DeInit();

    RST_CLK_HCLK_ClkSelection(RST_CLK_CPU_HCLK_CLK_SRC_HSI);

    SystemCoreClockUpdate();

    RST_CLK_HSE_Cmd(ENABLE);
    while (RST_CLK_HSE_GetStatus() != SUCCESS) { }


    ErrorCode = RST_CLK_HCLK_Init(&Clk);

    if (ErrorCode != SUCCESS) {
        while(1);
    }

    // RST_CLK_PER1_C2_ClkSelection(RST_CLK_PER1_C2_CLK_SRC_CPU_C1);
    // RST_CLK_PER1_C2_SetPrescaler(RST_CLK_PER1_C2_UART1, RST_CLK_PER1_PRESCALER_DIV_1);
    // RST_CLK_PER1_C2_Cmd(RST_CLK_PER1_C2_UART1, ENABLE);

    // RST_CLK_PCLKCmd(RST_CLK_PCLK_UART1, ENABLE);

    return;
}
