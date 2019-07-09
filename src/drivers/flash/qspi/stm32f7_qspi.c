/**
 * @file qspi.c
 * @brief QSPI flash driver (currently just for STM32F7Discovery)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.06.2019
 */

#include <embox/unit.h>

#if defined STM32F746xx
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_qspi.h"
#elif defined STM32F769xx
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_qspi.h"
#else
#error Unsupported platform
#endif

EMBOX_UNIT_INIT(stm32f7_qspi_init);

static int stm32f7_qspi_init(void) {
	BSP_QSPI_Init();

	BSP_QSPI_EnableMemoryMappedMode();

	return 0;
}
