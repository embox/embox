/**
 * @file
 *
 * @date 04.08.2019
 * @author Alexander Kalmuk
 */

#include <util/log.h>

#include <errno.h>
#include <assert.h>

#include <kernel/irq.h>

#include <framework/mod/options.h>

#include <drivers/audio/stm32h7_audio.h>

#define STM32_AUDIO_OUT_DMA_IRQ      OPTION_GET(NUMBER, audio_out_dma_irq)
static_assert(STM32_AUDIO_OUT_DMA_IRQ == AUDIO_OUT_SAIx_DMAx_IRQ, "");

#define STM32_AUDIO_IN_DMA_IRQ       OPTION_GET(NUMBER, audio_in_dma_irq)
static_assert(STM32_AUDIO_IN_DMA_IRQ == AUDIO_IN_SAIx_DMAx_IRQ, "");

#define STM32_AUDIO_IN_PDM_DMA_IRQ       OPTION_GET(NUMBER, audio_in_pdm_dma_irq)
static_assert(STM32_AUDIO_IN_PDM_DMA_IRQ == AUDIO_IN_SAI_PDMx_DMAx_IRQ, "");

static uint32_t  InState = 0;
static BSP_AUDIO_Init_t  AudioInInit;
static BSP_AUDIO_Init_t  AudioOutInit;

static irq_return_t stm32_audio_in_dma_interrupt(unsigned int irq_num,
		void *audio_dev) {
	extern SAI_HandleTypeDef haudio_in_sai;
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
	return IRQ_HANDLED;
}

STATIC_IRQ_ATTACH(STM32_AUDIO_IN_DMA_IRQ, stm32_audio_in_dma_interrupt, NULL);

static irq_return_t stm32_audio_in_pdm_dma_interrupt(unsigned int irq_num,
		void *audio_dev) {
	extern SAI_HandleTypeDef haudio_in_sai;
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
	return IRQ_HANDLED;
}

STATIC_IRQ_ATTACH(130, stm32_audio_in_pdm_dma_interrupt, NULL);

static irq_return_t stm32_audio_out_dma_interrupt(unsigned int irq_num,
		void *audio_dev) {
	extern SAI_HandleTypeDef haudio_out_sai;
	HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
	return IRQ_HANDLED;
}

STATIC_IRQ_ATTACH(STM32_AUDIO_OUT_DMA_IRQ, stm32_audio_out_dma_interrupt, NULL);

extern int32_t BSP_AUDIO_OUT_IN_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit);
int stm32h7_audio_init(void) {
	if (0 != irq_attach(STM32_AUDIO_IN_DMA_IRQ, stm32_audio_in_dma_interrupt,
				0, NULL, "stm32_audio_dma_in")) {
		log_error("irq_attach error");
	}
	if (0 != irq_attach(STM32_AUDIO_OUT_DMA_IRQ, stm32_audio_out_dma_interrupt,
				0, NULL, "stm32_audio_out")) {
		log_error("irq_attach error");
	}

	if (0 != irq_attach(STM32_AUDIO_IN_PDM_DMA_IRQ, stm32_audio_in_pdm_dma_interrupt,
				0, NULL, "stm32_audio_out")) {
		log_error("irq_attach error");
	}

	/* 16000 Hz, 16 bit, stereo. */
	AudioOutInit.Device = AUDIO_OUT_DEVICE_HEADPHONE;
	AudioOutInit.ChannelsNbr = 2;
	AudioOutInit.SampleRate = 16000;
	AudioOutInit.BitsPerSample = AUDIO_RESOLUTION_16B;
	AudioOutInit.Volume = 70;

//	AudioInInit.Device = AUDIO_IN_DEVICE_DIGITAL_MIC | AUDIO_OUT_DEVICE_HEADPHONE;
	AudioInInit.Device = AUDIO_IN_DEVICE_DIGITAL_MIC;
	AudioInInit.ChannelsNbr = 2;
	AudioInInit.SampleRate = 16000;
	AudioInInit.BitsPerSample = AUDIO_RESOLUTION_16B;
	AudioInInit.Volume = 70;

	BSP_AUDIO_OUT_Init(0, &AudioOutInit);
//	BSP_AUDIO_OUT_IN_Init(0, &AudioInInit);

	/* Initialize Audio Recorder with 2 channels to be used */
	BSP_AUDIO_IN_Init(1, &AudioInInit);
	BSP_AUDIO_IN_GetState(1, &InState);

//	BSP_AUDIO_OUT_SetDevice(0, AUDIO_OUT_DEVICE_HEADPHONE);
	BSP_AUDIO_OUT_SetVolume(0, 100);
	return 0;
}
