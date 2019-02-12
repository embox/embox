/**
 * @file
 * @brief
 *
 * @date 06.09.18
 * @author Alexander Kalmuk
 */

#ifndef STM32_AUDIO_CONF_F4_H_
#define STM32_AUDIO_CONF_F4_H_

#include "stm32f4_discovery_audio.h"

#define STM32_AUDIO_DMA_IRQ 63

static_assert(STM32_AUDIO_DMA_IRQ == (I2S3_DMAx_IRQ + 16));

static irq_return_t stm32_audio_dma_interrupt(unsigned int irq_num,
		void *dev_id) {
	extern I2S_HandleTypeDef hAudioOutI2s;

	HAL_DMA_IRQHandler(hAudioOutI2s.hdmatx);
	return IRQ_HANDLED;
}

#endif /* STM32_AUDIO_CONF_F4_H_ */
