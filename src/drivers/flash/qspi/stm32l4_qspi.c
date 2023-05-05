/**
 * @file stm32l4_qspi.c
 * @brief QSPI flash driver (currently just for STM32L4Discovery)
 * @author Andrew Bursian
 * @date 04.05.2023
 */

#include <embox/unit.h>

#if defined USE_STM32L475E_IOT01
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_qspi.h" // includes "../Components/mx25r6435f/mx25r6435f.h"
#else
#error Unsupported platform
#endif

EMBOX_UNIT_INIT(stm32l4_qspi_init);

static int stm32l4_qspi_init(void) {
	BSP_QSPI_Init();

	BSP_QSPI_EnableMemoryMappedMode();

	return 0;
}
