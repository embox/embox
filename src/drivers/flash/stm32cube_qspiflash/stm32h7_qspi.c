/**
 * @file qspi.c
 * @brief QSPI flash driver (currently just for STM32F7Discovery)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.06.2019
 */

#include <embox/unit.h>

#include "stm32h745i_discovery.h"
#include "stm32h745i_discovery_qspi.h"

EMBOX_UNIT_INIT(stm32h7_qspi_init);

static int stm32h7_qspi_init(void) {
	BSP_QSPI_Init_t qspi_init;

	qspi_init.InterfaceMode = MT25TL01G_QPI_MODE;
	qspi_init.TransferRate = MT25TL01G_DTR_TRANSFER;
	qspi_init.DualFlashMode = MT25TL01G_DUALFLASH_ENABLE;

	//BSP_QSPI_DisableMemoryMappedMode(0);
	//BSP_QSPI_DeInit(0);
	BSP_QSPI_Init(0, &qspi_init);
	BSP_QSPI_EnableMemoryMappedMode(0);

	return 0;
}
