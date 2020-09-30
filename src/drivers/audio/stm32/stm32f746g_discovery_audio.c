/**
 * @file
 *
 * @date 04.08.2019
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <util/log.h>
#include <assert.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>

#include <drivers/audio/stm32f7_audio.h>

#define STM32_AUDIO_OUT_DMA_IRQ      OPTION_GET(NUMBER, audio_out_dma_irq)
static_assert(STM32_AUDIO_OUT_DMA_IRQ == AUDIO_OUT_SAIx_DMAx_IRQ);

#define STM32_AUDIO_IN_DMA_IRQ       OPTION_GET(NUMBER, audio_in_dma_irq)
static_assert(STM32_AUDIO_IN_DMA_IRQ == AUDIO_IN_SAIx_DMAx_IRQ);

static irq_return_t stm32_audio_in_dma_interrupt(unsigned int irq_num,
		void *audio_dev) {
	extern SAI_HandleTypeDef haudio_in_sai;
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
	return IRQ_HANDLED;
}

STATIC_IRQ_ATTACH(STM32_AUDIO_IN_DMA_IRQ, stm32_audio_in_dma_interrupt, NULL);

static irq_return_t stm32_audio_out_dma_interrupt(unsigned int irq_num,
		void *audio_dev) {
	extern SAI_HandleTypeDef haudio_out_sai;
	HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
	return IRQ_HANDLED;
}

STATIC_IRQ_ATTACH(STM32_AUDIO_OUT_DMA_IRQ, stm32_audio_out_dma_interrupt, NULL);

/* XXX
 * STM32 Cube suppose only 3 possible audio configurations:
 * 1. BSP_AUDIO_OUT_Play - only OUTPUT
 * 2. BSP_AUDIO_IN_Record - only INPUT
 * 3. BSP_AUDIO_IN_OUT_Init - BOTH
 * So the only one possibility to use both audio in and audio out is
 * to use BSP_AUDIO_IN_OUT_Init, which makes run BOTH at any time. It means
 * that even we use 'record' audio out will be activated too.
 * WORKAROUND: Run both IN and OUT everytime, and actually do not stop them
 * even if auddio_dev->stop() is called.
 */
int stm32f7_audio_init(void) {
	if (0 != irq_attach(STM32_AUDIO_IN_DMA_IRQ, stm32_audio_in_dma_interrupt,
				0, NULL, "stm32_audio_dma_in")) {
		log_error("irq_attach error");
	}
	if (0 != irq_attach(STM32_AUDIO_OUT_DMA_IRQ, stm32_audio_out_dma_interrupt,
				0, NULL, "stm32_audio_out")) {
		log_error("irq_attach error");
	}

	/* 16000 Hz, 16 bit, stereo. */
	if (0 != BSP_AUDIO_IN_OUT_Init(
				INPUT_DEVICE_DIGITAL_MICROPHONE_2,
				OUTPUT_DEVICE_HEADPHONE,
				16000, 16, 2)) {
		log_error("BSP_AUDIO_IN_OUT_Init error");
		return -1;
	}

	return 0;
}
