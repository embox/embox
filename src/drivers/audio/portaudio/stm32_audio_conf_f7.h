/**
 * @file
 * @brief
 *
 * @date 06.09.18
 * @author Alexander Kalmuk
 */

#ifndef STM32_AUDIO_CONF_F7_H_
#define STM32_AUDIO_CONF_F7_H_

#include "stm32746g_discovery_audio.h"
#include <framework/mod/options.h>
#include <module/embox/driver/audio/stm32f7_pa_cube.h>

#define STM32_AUDIO_DMA_IRQ    \
	OPTION_MODULE_GET(embox__driver__audio__stm32f7_pa_cube, NUMBER, audio_dma_irq)
static_assert(STM32_AUDIO_DMA_IRQ == AUDIO_OUT_SAIx_DMAx_IRQ);

static irq_return_t stm32_audio_dma_interrupt(unsigned int irq_num,
		void *dev_id) {
	extern SAI_HandleTypeDef haudio_out_sai;

	HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
	return IRQ_HANDLED;
}

#endif /* STM32_AUDIO_CONF_F7_H_ */
