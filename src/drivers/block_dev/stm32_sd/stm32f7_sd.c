/**
 * @file
 * @brief SD-Card driver for stm32f7-Discovery using SDIO.
 *
 * @author  Denis Deryugin
 * @date    26 Jan 2015
 */

#include <string.h>
#include <assert.h>

#include <drivers/block_dev.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <util/log.h>

#include "stm32f7_discovery_sd.h"

static int stm32f7_sd_init(void *arg);
static int stm32f7_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size);
static int stm32f7_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);
static int stm32f7_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);

#define STM32F7_SD_DEVNAME "stm32f7_sd_card"
#define SD_BUF_SIZE OPTION_GET(NUMBER, sd_buf_size)

#if OPTION_GET(BOOLEAN, use_local_buf)
#define USE_LOCAL_BUF
#endif

static irq_return_t stm32_dma_rx_irq(unsigned int irq_num,
		void *audio_dev) {
	extern SD_HandleTypeDef uSdHandle;
	HAL_DMA_IRQHandler(uSdHandle.hdmarx);
	return IRQ_HANDLED;
}

static irq_return_t stm32_dma_tx_irq(unsigned int irq_num,
		void *audio_dev) {
	extern SD_HandleTypeDef uSdHandle;
	HAL_DMA_IRQHandler(uSdHandle.hdmatx);
	return IRQ_HANDLED;
}

static irq_return_t stm32_sdmmc_irq(unsigned int irq_num,
		void *audio_dev) {
	extern SD_HandleTypeDef uSdHandle;
	HAL_SD_IRQHandler(&uSdHandle);
	return IRQ_HANDLED;
}

block_dev_driver_t stm32f7_sd_driver = {
	.name  = STM32F7_SD_DEVNAME,
	.ioctl = stm32f7_sd_ioctl,
	.read  = stm32f7_sd_read,
	.write = stm32f7_sd_write,
	.probe = stm32f7_sd_init,
};

BLOCK_DEV_DEF(STM32F7_SD_DEVNAME, &stm32f7_sd_driver);

static int stm32f7_sd_init(void *arg) {
	struct block_dev *bdev;

	if (!block_dev_lookup(STM32F7_SD_DEVNAME)) {
		log_error("Block device not found\n");
		return -1;
	}
	if (0 != irq_attach(STM32_DMA_RX_IRQ,
				stm32_dma_rx_irq,
				0, NULL, "stm32_dma_rx_irq")) {
		log_error("irq_attach error");
		return -1;
	}
	if (0 != irq_attach(STM32_DMA_TX_IRQ,
				stm32_dma_tx_irq,
				0, NULL, "stm32_dma_tx_irq")) {
		log_error("irq_attach error");
		return -1;
	}
	if (0 != irq_attach(STM32_SDMMC_IRQ,
				stm32_sdmmc_irq,
				0, NULL, "stm32_sdmmc_irq")) {
		log_error("irq_attach error");
		return -1;
	}

	/* SDMMC2 irq priority should be higher that DMA due to
	 * STM32Cube implementation. */
	irqctrl_set_prio(STM32_DMA_RX_IRQ, 10);
	irqctrl_set_prio(STM32_DMA_TX_IRQ, 10);
	irqctrl_set_prio(STM32_SDMMC_IRQ, 11);

	if (BSP_SD_Init() == MSD_OK) {
		bdev = block_dev_create(STM32F7_SD_DEVNAME, &stm32f7_sd_driver, NULL);
		bdev->size = stm32f7_sd_ioctl(bdev, IOCTL_GETDEVSIZE, NULL, 0);
		return 0;
	} else if (BSP_SD_IsDetected() != SD_PRESENT) {
		/* microSD Card is not inserted, do nothing. */
		return 0;
	} else {
		log_error("BSP_SD_Init error\n");
		irq_detach(STM32_DMA_RX_IRQ, NULL);
		irq_detach(STM32_DMA_TX_IRQ, NULL);
		irq_detach(STM32_SDMMC_IRQ, NULL);
		return -1;
	}
}

static int stm32f7_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size) {
	SD_CardInfo info;

	BSP_SD_GetCardInfo(&info);

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return info.CardCapacity;
	case IOCTL_GETBLKSIZE:
		return info.CardBlockSize;
	default:
		return -1;
	}
}
#ifdef USE_LOCAL_BUF
static uint8_t sd_buf[SD_BUF_SIZE];
#endif
static int stm32f7_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	assert(count <= SD_BUF_SIZE);
	int res = -1;
	size_t bsize = bdev->block_size;
#ifdef USE_LOCAL_BUF
	res = BSP_SD_ReadBlocks_DMA((uint32_t *) sd_buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	if (res == -1) {
		log_error("BSP_SD_ReadBlocks_DMA failed, blkno=%d\n", blkno);
	}
	memcpy(buf, sd_buf, bsize);
#else
	res = BSP_SD_ReadBlocks_DMA((uint32_t *) buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	if (res == -1) {
		log_error("BSP_SD_ReadBlocks_DMA failed, blkno=%d\n", blkno);
	}
#endif

	return res;
}

static int stm32f7_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	assert(count <= SD_BUF_SIZE);
	int res;
	size_t bsize = bdev->block_size;
#ifdef USE_LOCAL_BUF
	memcpy(sd_buf, buf, bsize);
	res = BSP_SD_WriteBlocks_DMA((uint32_t *) sd_buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	if (res == -1) {
		log_error("BSP_SD_WriteBlocks_DMA failed, blkno=%d\n", blkno);
	}
#else
	res = BSP_SD_WriteBlocks_DMA((uint32_t *) buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	if (res == -1) {
		log_error("BSP_SD_WriteBlocks_DMA failed, blkno=%d\n", blkno);
	}
#endif
	return res;
}
