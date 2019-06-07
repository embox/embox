/**
 * @file qspi.c
 * @brief QSPI flash driver (currently just for STM32F7Discovery)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.06.2019
 */

#include <embox/unit.h>

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_qspi.h"

EMBOX_UNIT_INIT(stm32f7_qspi_init);

static int stm32f7_qspi_init(void) {
	BSP_QSPI_Init();

	BSP_QSPI_EnableMemoryMappedMode();

	return 0;
}
