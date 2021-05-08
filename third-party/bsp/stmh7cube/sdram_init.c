/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2014
 */

#include <assert.h>
#include <stdint.h>

#include <system_stm32h7xx.h>
#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_cortex.h>

#include <framework/mod/options.h>


#include <stm32h745i_discovery.h>
#include <stm32h745i_discovery_sdram.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(sdram_init);

#define FMC_SWAP OPTION_GET(BOOLEAN, fmc_swap)

static int sdram_init(void) {
	BSP_SDRAM_Init(0);

	if (FMC_SWAP) {
		//HAL_SetFMCMemorySwappingConfig(FMC_BCR1_BMAP_0);
		HAL_SetFMCMemorySwappingConfig(FMC_SWAPBMAP_SDRAMB2);
	}
	return 0;
}
