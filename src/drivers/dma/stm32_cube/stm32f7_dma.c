/**
 * @file
 * @brief
 *
 * @date    26.07.2020
 * @author  Alexander Kalmuk
 */

#include <string.h>
#include <assert.h>
#include <util/log.h>
#include <kernel/irq.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <drivers/dma/dma.h>

#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_dma.h>

EMBOX_UNIT_INIT(dma_init);

#define DMA_MAX_SIZE (64 * 1024 - 1)

#define DMA2_STREAM0_IRQ   OPTION_GET(NUMBER, dma2_stream0_irq)
static_assert(DMA2_STREAM0_IRQ == DMA2_Stream0_IRQn);

static DMA_HandleTypeDef dma_handle;

static uint32_t dma_dst, dma_src;
static unsigned dma_words;

static irq_return_t dma2_stream0_irq_handler(unsigned int irq_num, void *dev_id) {
	HAL_DMA_IRQHandler(&dma_handle);

	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(DMA2_STREAM0_IRQ, dma2_stream0_irq_handler, NULL);

static void dma_xfer_complete(DMA_HandleTypeDef *hdma) {
	int words;

	(void) hdma;

	if (dma_words <= DMA_MAX_SIZE) {
		dma_words = 0;
		return;
	}

	dma_words -= DMA_MAX_SIZE;
	dma_dst += DMA_MAX_SIZE * 4;
	dma_src += DMA_MAX_SIZE * 4;

	words = dma_words > DMA_MAX_SIZE ? DMA_MAX_SIZE : dma_words;

	if (HAL_DMA_Start_IT(&dma_handle, dma_src, dma_dst, words) != HAL_OK) {
		log_error("HAL_DMA_Start_IT error!");
	}
}

int dma_in_progress(int dma_chan) {
	assert(dma_chan == 0);

	return (HAL_DMA_GetState(&dma_handle) != HAL_DMA_STATE_READY);
}

static void dma_init_chan(DMA_HandleTypeDef *dma_handle, int chan) {
	dma_handle->Init.Direction = DMA_MEMORY_TO_MEMORY;
	dma_handle->Init.PeriphInc = DMA_PINC_ENABLE;
	dma_handle->Init.MemInc = DMA_MINC_ENABLE;
	dma_handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dma_handle->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	dma_handle->Init.Mode = DMA_NORMAL;
	dma_handle->Init.Priority = DMA_PRIORITY_HIGH;
	dma_handle->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	dma_handle->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	dma_handle->Init.MemBurst = DMA_MBURST_SINGLE;
	dma_handle->Init.PeriphBurst = DMA_PBURST_SINGLE;

	dma_handle->Init.Channel = chan;
}

int dma_config(int dma_chan) {
	assert(dma_chan == 0);

	__HAL_RCC_DMA2_CLK_ENABLE();

	dma_init_chan(&dma_handle, dma_chan);

	dma_handle.Instance = DMA2_Stream0;
	if (HAL_DMA_Init(&dma_handle) != HAL_OK) {
		log_error("DMA init failed");
		return -1;
	}
	HAL_DMA_RegisterCallback(&dma_handle, HAL_DMA_XFER_CPLT_CB_ID,
			dma_xfer_complete);

	return 0;
}

int dma_transfer(int dma_chan, uint32_t dst, uint32_t src, int words) {
	assert(dma_chan == 0);

	log_debug("chan=%d, dst=0x%08x, src=0x%08x, words=%d",
		dma_chan, dst, src, words);

	dma_dst = dst;
	dma_src = src;
	dma_words = words;

	if (words > DMA_MAX_SIZE) {
		words = DMA_MAX_SIZE;
	}

	if (HAL_DMA_Start_IT(&dma_handle, src, dst, words) != HAL_OK) {
		log_error("HAL_DMA_Start_IT error!");
		return -1;
	}

	return 0;
}

static int dma_init(void) {
	int ret = 0;

	ret = irq_attach(DMA2_STREAM0_IRQ, dma2_stream0_irq_handler,
			0, NULL, "dma2_stream0");
	if (ret < 0) {
		log_error("irq_attach failed\n");
	}

	return ret;
}
