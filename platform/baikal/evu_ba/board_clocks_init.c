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
