/**
 * @file qspi.c
 * @brief QSPI flash driver (currently just for STM32F7Discovery)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.06.2019
 */

#include <embox/unit.h>

#if defined USE_BL475EIOT01A
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_qspi.h"
#else
#error Unsupported platform
#endif

EMBOX_UNIT_INIT(stm32l4_qspi_init);

static int stm32l4_qspi_init(void) {
	BSP_QSPI_Init();

	BSP_QSPI_EnableMemoryMappedMode();

	return 0;
}
