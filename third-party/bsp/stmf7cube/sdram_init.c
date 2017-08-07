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
#include <stm32746g_discovery.h>
#include <stm32f7xx_hal_cortex.h>

#include <framework/mod/options.h>

#include <stm32746g_discovery_sdram.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(sdram_init);

static int sdram_init(void) {
  BSP_SDRAM_Init();
  return 0;
}
