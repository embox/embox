/**
 * @file
 * @brief SD-Card driver for stm32f7-Discovery using SDIO.
 *
 * @author  Denis Deryugin
 * @date    26 Jan 2015
 */

#include <util/log.h>

#include <errno.h>
#include <string.h>
#include <assert.h>

#include <drivers/block_dev.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>

#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched.h>

#include "stm32f7_discovery_sd.h"

#define STM32F7_SD_DEVNAME "sd_card"
#define SD_BUF_SIZE OPTION_GET(NUMBER, sd_buf_size)


#if OPTION_GET(BOOLEAN, use_local_buf)
#define USE_LOCAL_BUF
static uint8_t sd_buf[SD_BUF_SIZE];
#endif

static volatile int dma_transfer_active = 0;
static volatile struct schedee *dma_transfer_thread = NULL;

static irq_return_t stm32_dma_rx_irq(unsigned int irq_num,
		void *audio_dev) {
	extern SD_HandleTypeDef uSdHandle;
	HAL_DMA_IRQHandler(uSdHandle.hdmarx);

	if (!dma_transfer_active) {
		dma_transfer_active = 1;
		sched_wakeup((struct schedee *)dma_transfer_thread);
	}

	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(STM32_DMA_RX_IRQ, stm32_dma_rx_irq, NULL);

static irq_return_t stm32_dma_tx_irq(unsigned int irq_num,
		void *audio_dev) {
	extern SD_HandleTypeDef uSdHandle;
	HAL_DMA_IRQHandler(uSdHandle.hdmatx);

	if (!dma_transfer_active) {
		dma_transfer_active = 1;
		sched_wakeup((struct schedee *)dma_transfer_thread);
	}

	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(STM32_DMA_TX_IRQ, stm32_dma_tx_irq, NULL);

static irq_return_t stm32_sdmmc_irq(unsigned int irq_num,
		void *audio_dev) {
	extern SD_HandleTypeDef uSdHandle;
	HAL_SD_IRQHandler(&uSdHandle);
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(STM32_SDMMC_IRQ, stm32_sdmmc_irq, NULL);

static int stm32_transfer_prepare(void) {
	dma_transfer_active = 0;
	dma_transfer_thread = &thread_self()->schedee;

	return 0;
}

static int stm32_transfer_wait(void) {
	int res;

	res = SCHED_WAIT_TIMEOUT(dma_transfer_active, 100);
	if (res != 0) {
		log_error("timeout");
		return -ETIMEDOUT;
	}
	return 0;
}

static int stm32f7_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size) {
	HAL_SD_CardInfoTypeDef info;

	BSP_SD_GetCardInfo(&info);

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return info.BlockNbr * info.BlockSize;
	case IOCTL_GETBLKSIZE:
		return info.BlockSize;
	default:
		return -1;
	}
}

static int stm32_sd_read_block(char *buf, blkno_t blkno) {
	int res;

	res = stm32_transfer_prepare();
	if (res) {
		return res;
	}

	res = BSP_SD_ReadBlocks_DMA((uint32_t *) buf, blkno, 1);
	if (res) {
		log_error("BSP_SD_ReadBlocks_DMA failed, blkno=%d\n", blkno);
		return -1;
	}

	res = stm32_transfer_wait();
	if (res) {
		return res;
	}

	return BLOCKSIZE;
}


static int stm32f7_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	int res = -1;
	char *tmp_buf;
	size_t bsize = bdev->block_size;

	assert(bsize == BLOCKSIZE);

#ifdef USE_LOCAL_BUF
	tmp_buf = (char *)sd_buf;
#else
	tmp_buf = buf;
#endif
	res = stm32_sd_read_block(tmp_buf, blkno);
	if (res < 0) {
		return res;
	}
#ifdef USE_LOCAL_BUF
	memcpy(buf, sd_buf, bsize);
#endif

	return bsize;
}


static int stm32_sd_write_block(char *buf, blkno_t blkno) {
	int res;

	res = stm32_transfer_prepare();
	if (res) {
		return res;
	}

	res = BSP_SD_WriteBlocks_DMA((uint32_t *) buf, blkno, 1);
	if (res != 0) {
		log_error("BSP_SD_WriteBlocks_DMA failed, blkno=%d\n", blkno);
		return -1;
	}

	res = stm32_transfer_wait();
	if (res) {
		return res;
	}

	return BLOCKSIZE;
}

static int stm32f7_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	char *tmp_buf;
	int res;
	size_t bsize = bdev->block_size;

	assert(bsize == BLOCKSIZE);

#ifdef USE_LOCAL_BUF
	memcpy(sd_buf, buf, bsize);
	tmp_buf = (char *)sd_buf;
#else
	tmp_buf = buf;
#endif
	res = stm32_sd_write_block(tmp_buf, blkno);
	if (res < 0) {
		return res;
	}

	return bsize;
}

static const struct block_dev_ops stm32f7_sd_driver;
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
		return -1;
	}
}

static const struct block_dev_ops stm32f7_sd_driver = {
	.name  = STM32F7_SD_DEVNAME,
	.ioctl = stm32f7_sd_ioctl,
	.read  = stm32f7_sd_read,
	.write = stm32f7_sd_write,
	.probe = stm32f7_sd_init,
};

BLOCK_DEV_DRIVER_DEF(STM32F7_SD_DEVNAME, &stm32f7_sd_driver);
