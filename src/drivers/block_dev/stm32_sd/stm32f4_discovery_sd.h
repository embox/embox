#ifndef STM32F4_DISCOVERY_SD_H_
#define STM32F4_DISCOVERY_SD_H_

#include <assert.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_sd.h"
#include "stm324xg_eval_sd.h"

#include <framework/mod/options.h>

#define STM32_DMA_RX_IRQ   OPTION_GET(NUMBER, dma_rx_irq)
static_assert(STM32_DMA_RX_IRQ == DMA2_Stream3_IRQn, "");

#define STM32_DMA_TX_IRQ   OPTION_GET(NUMBER, dma_tx_irq)
static_assert(STM32_DMA_TX_IRQ == DMA2_Stream6_IRQn, "");

#define STM32_SDMMC_IRQ    OPTION_GET(NUMBER, dma_sdmmc_irq)
//static_assert(STM32_SDMMC_IRQ == EXTI15_10_IRQn, "");

#endif /* STM32F4_DISCOVERY_SD_H_ */
