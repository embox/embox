/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2014
 */

#include <assert.h>
#include <stdint.h>

#include <system_stm32f7xx.h>
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_cortex.h>

#include <framework/mod/options.h>

#if defined STM32F746xx
#include <stm32746g_discovery.h>
#include <stm32746g_discovery_sdram.h>
#elif defined STM32F769xx
#include <stm32f769i_discovery.h>
#include <stm32f769i_discovery_sdram.h>
#else
#error Unsupported platform
#endif

#include <embox/unit.h>

EMBOX_UNIT_INIT(sdram_init);

#define FMC_SWAP OPTION_GET(BOOLEAN, fmc_swap)

static int sdram_init(void) {
	BSP_SDRAM_Init();

	if (FMC_SWAP) {
		HAL_EnableFMCMemorySwapping();
	}
	return 0;
}
