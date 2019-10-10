/**
 * @file
 *
 * @date 04.08.2019
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <assert.h>
#include <util/log.h>
#include <kernel/irq.h>

#include <drivers/audio/stm32f7_audio.h>

#define STM32_AUDIO_OUT_DMA_IRQ (AUDIO_OUT_SAIx_DMAx_IRQ + 16)
/* Audio Input interrupts */
#define STM32_DFSDM_TOP_LEFT_IRQ     (DMA2_Stream0_IRQn + 16)
#define STM32_DFSDM_TOP_RIGHT_IRQ    (DMA2_Stream5_IRQn + 16)
#define STM32_DFSDM_BOTTOM_LEFT_IRQ  (DMA2_Stream6_IRQn + 16)
#define STM32_DFSDM_BOTTOM_RIGHT_IRQ (DMA2_Stream7_IRQn + 16)

#define SCRATCH_BUFF_SIZE 1024
static int32_t audio_scratch_buffer[SCRATCH_BUFF_SIZE];

static irq_return_t stm32_audio_out_dma_irq(unsigned int irq_num,
		void *audio_dev) {
	extern SAI_HandleTypeDef haudio_out_sai;
	HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
	return IRQ_HANDLED;
}
static_assert(73 == STM32_AUDIO_OUT_DMA_IRQ);
STATIC_IRQ_ATTACH(73, stm32_audio_out_dma_irq, NULL);

static irq_return_t stm32_dfsdm_top_left_irq(unsigned int irq_num,
		void *audio_dev) {
	extern DFSDM_Filter_HandleTypeDef hAudioInTopLeftFilter;
	HAL_DMA_IRQHandler(hAudioInTopLeftFilter.hdmaReg);
	return IRQ_HANDLED;
}
static_assert(72 == STM32_DFSDM_TOP_LEFT_IRQ);
STATIC_IRQ_ATTACH(72, stm32_dfsdm_top_left_irq, NULL);

static irq_return_t stm32_dfsdm_top_right_irq(unsigned int irq_num,
		void *audio_dev) {
	extern DFSDM_Filter_HandleTypeDef hAudioInTopRightFilter;
	HAL_DMA_IRQHandler(hAudioInTopRightFilter.hdmaReg);
	return IRQ_HANDLED;
}
static_assert(84 == STM32_DFSDM_TOP_RIGHT_IRQ);
STATIC_IRQ_ATTACH(84, stm32_dfsdm_top_right_irq, NULL);

static irq_return_t stm32_dfsdm_bottom_left_irq(unsigned int irq_num,
		void *audio_dev) {
	extern DFSDM_Filter_HandleTypeDef hAudioInButtomLeftFilter;
	HAL_DMA_IRQHandler(hAudioInButtomLeftFilter.hdmaReg);
	return IRQ_HANDLED;
}
static_assert(85 == STM32_DFSDM_BOTTOM_LEFT_IRQ);
STATIC_IRQ_ATTACH(85, stm32_dfsdm_bottom_left_irq, NULL);

static irq_return_t stm32_dfsdm_bottom_right_irq(unsigned int irq_num,
		void *audio_dev) {
	extern DFSDM_Filter_HandleTypeDef hAudioInButtomRightFilter;
	HAL_DMA_IRQHandler(hAudioInButtomRightFilter.hdmaReg);
	return IRQ_HANDLED;
}
static_assert(86 == STM32_DFSDM_BOTTOM_RIGHT_IRQ);
STATIC_IRQ_ATTACH(86, stm32_dfsdm_bottom_right_irq, NULL);

int stm32f7_audio_init(void) {
	if (0 != irq_attach(STM32_AUDIO_OUT_DMA_IRQ,
				stm32_audio_out_dma_irq,
				0, NULL, "stm32_audio_out")) {
		log_error("irq_attach error");
	}
	if (0 != irq_attach(STM32_DFSDM_TOP_LEFT_IRQ,
				stm32_dfsdm_top_left_irq,
				0, NULL, "stm32_audio_in_top_left")) {
		log_error("irq_attach error");
	}
	if (0 != irq_attach(STM32_DFSDM_TOP_RIGHT_IRQ,
				stm32_dfsdm_top_right_irq,
				0, NULL, "stm32_audio_in_top_right")) {
		log_error("irq_attach error");
	}
	if (0 != irq_attach(STM32_DFSDM_BOTTOM_LEFT_IRQ,
				stm32_dfsdm_bottom_left_irq,
				0, NULL, "stm32_audio_in_bottom_left")) {
		log_error("irq_attach error");
	}
	if (0 != irq_attach(STM32_DFSDM_BOTTOM_RIGHT_IRQ,
				stm32_dfsdm_bottom_right_irq,
				0, NULL, "stm32_audio_in_bottom_right")) {
		log_error("irq_attach error");
	}

	if (AUDIO_OK != BSP_AUDIO_IN_Init(BSP_AUDIO_FREQUENCY_16K,
			DEFAULT_AUDIO_IN_BIT_RESOLUTION,
			DEFAULT_AUDIO_IN_CHANNEL_NBR)) {
		log_error("BSP_AUDIO_IN_Init error");
		return -1;
	}
	BSP_AUDIO_IN_AllocScratch(audio_scratch_buffer, SCRATCH_BUFF_SIZE);

	if (AUDIO_OK != BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 
			90, BSP_AUDIO_FREQUENCY_16K)) {
		log_error("BSP_AUDIO_OUT_Init error");
		return -1;
	}

	return 0;
}
