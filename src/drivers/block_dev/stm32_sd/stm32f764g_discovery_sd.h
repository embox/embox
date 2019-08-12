/**
 * @file
 * @brief SD-Card driver for stm32f7-Discovery using SDIO.
 *
 * @author  Denis Deryugin
 * @date    26 Jan 2015
 */

#ifndef STM32F764G_DISCOVERY_SD_H_
#define STM32F764G_DISCOVERY_SD_H_

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_sd.h"
#include "stm32746g_discovery_sd.h"

#define STM32_DMA_RX_IRQ   (DMA2_Stream3_IRQn + 16)
#define STM32_DMA_TX_IRQ   (DMA2_Stream6_IRQn + 16)
#define STM32_SDMMC_IRQ    (SDMMC1_IRQn + 16)

#endif /* STM32F764G_DISCOVERY_SD_H_ */
