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

#include <kernel/irq.h>
#include <arm/cpu_cache.h>

#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched.h>

#include "stm32cube_bsp_sd.h"

#include <framework/mod/options.h>

#define STM32F7_SD_DEVNAME "sd_card"

#define USE_LOCAL_BUF OPTION_GET(BOOLEAN, use_local_buf)
#define USE_IRQ       OPTION_GET(BOOLEAN,use_irq)

#if USE_LOCAL_BUF
static uint8_t sd_buf[BLOCKSIZE] __attribute__ ((aligned (4))) SRAM_NOCACHE_SECTION;
#endif

#define DMA_TRANSFER_STATE_IDLE      (0)
#define DMA_TRANSFER_STATE_RX_START  (2)
#define DMA_TRANSFER_STATE_RX_FIN    (3)
#define DMA_TRANSFER_STATE_TX_START  (4)
#define DMA_TRANSFER_STATE_TX_FIN    (5)

#if USE_IRQ
#define stm32_transfer_read(buf, blkno, num)  \
	BSP_SD_ReadBlocks_DMA(buf, blkno, num)
#define stm32_transfer_write(buf, blkno, num) \
	BSP_SD_WriteBlocks_DMA(buf, blkno, num)

static volatile int dma_transfer_state = DMA_TRANSFER_STATE_IDLE;
static volatile struct schedee *dma_transfer_thread = NULL;

extern SD_HandleTypeDef uSdHandle;

static irq_return_t stm32_dma_rx_irq(unsigned int irq_num, void *dev) {
	HAL_DMA_IRQHandler(uSdHandle.hdmarx);

	if (dma_transfer_state == DMA_TRANSFER_STATE_RX_START) {
		dma_transfer_state = DMA_TRANSFER_STATE_RX_FIN;
		sched_wakeup((struct schedee *)dma_transfer_thread);
	}

	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(STM32_DMA_RX_IRQ, stm32_dma_rx_irq, NULL);

static irq_return_t stm32_dma_tx_irq(unsigned int irq_num, void *dev) {
	HAL_DMA_IRQHandler(uSdHandle.hdmatx);

	if (dma_transfer_state == DMA_TRANSFER_STATE_TX_START) {
		dma_transfer_state = DMA_TRANSFER_STATE_TX_FIN;
		sched_wakeup((struct schedee *)dma_transfer_thread);
	}

	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(STM32_DMA_TX_IRQ, stm32_dma_tx_irq, NULL);

static irq_return_t stm32_sdmmc_irq(unsigned int irq_num, void *dev) {
	HAL_SD_IRQHandler(&uSdHandle);
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(STM32_SDMMC_IRQ, stm32_sdmmc_irq, NULL);

static int stm32_transfer_prepare(int state) {
	if (dma_transfer_state != DMA_TRANSFER_STATE_IDLE) {
		return -EBUSY;
	}

	dma_transfer_state = state;
	dma_transfer_thread = &thread_self()->schedee;

	return 0;
}
extern uint8_t BSP_SD_GetCardState(void);
static int stm32_transfer_wait(int state) {
	int res;

	res = SCHED_WAIT_TIMEOUT((dma_transfer_state == state), 100);
	dma_transfer_state = DMA_TRANSFER_STATE_IDLE;

	if (res != 0) {
		log_error("timeout");
		res = -ETIMEDOUT;
	}

	while (BSP_SD_GetCardState() != SD_TRANSFER_OK);

	return res;
}

static int stm32_transfer_abort(int state) {
	dma_transfer_state = state;

	return 0;
}
#else
static inline int stm32_transfer_read(uint32_t *buf, int blkno, int num) {
	ipl_t ipl;
	int res;
	ipl = ipl_save();
	res = BSP_SD_ReadBlocks(buf, blkno, num, 10);
	ipl_restore(ipl);

	return res;
}

#define stm32_transfer_write(buf, blkno, num) \
	BSP_SD_WriteBlocks(buf, blkno, num, 10)

static int stm32_transfer_prepare(int state) {
	return 0;
}
static int stm32_transfer_wait(int state) {
	while (BSP_SD_GetCardState() != SD_TRANSFER_OK);

	return 0;
}

static int stm32_transfer_abort(int state) {
	return 0;
}
#endif

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

	res = stm32_transfer_prepare(DMA_TRANSFER_STATE_RX_START);
	if (res) {
		return res;
	}

	res = stm32_transfer_read((uint32_t *) buf, blkno, 1);
	if (res) {
		log_error("stm32_transfer_read failed, blkno=%d, res%dn", blkno,res);
		return -1;
	}

	res = stm32_transfer_wait(DMA_TRANSFER_STATE_RX_FIN);
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

#if USE_LOCAL_BUF
	tmp_buf = (char *)sd_buf;
#else
	tmp_buf = buf;
#endif
	res = stm32_sd_read_block(tmp_buf, blkno);
	if (res < 0) {
		return res;
	}
#if USE_LOCAL_BUF
	memcpy(buf, sd_buf, bsize);
#endif

	return bsize;
}


static int stm32_sd_write_block(char *buf, blkno_t blkno) {
	int res;

	res = stm32_transfer_prepare(DMA_TRANSFER_STATE_TX_START);
	if (res) {
		return res;
	}

	res = stm32_transfer_write((uint32_t *) buf, blkno, 1);
	if (res != 0) {
		log_error("stm32_transfer_write failed, blkno=%d res=%d", blkno, res);
		stm32_transfer_abort(DMA_TRANSFER_STATE_IDLE);
		return -1;
	}

	res = stm32_transfer_wait(DMA_TRANSFER_STATE_TX_FIN);
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

#if USE_LOCAL_BUF
	memcpy(sd_buf, buf, bsize);
	tmp_buf = (char *)sd_buf;
#else
	tmp_buf = buf;
#endif
	res = stm32_sd_write_block(tmp_buf, blkno);
	log_debug("written=%d res %d", blkno, res);
	if (res < 0) {
		return res;
	}

	return bsize;
}

static const struct block_dev_ops stm32f7_sd_driver;
static int stm32f7_sd_init(struct block_dev *bdev, void *arg) {

	if (!block_dev_lookup(STM32F7_SD_DEVNAME)) {
		log_error("Block device not found");
		return -1;
	}
#if USE_IRQ
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
#endif
	if (BSP_SD_Init() == MSD_OK) {
		bdev = block_dev_create(STM32F7_SD_DEVNAME, &stm32f7_sd_driver, NULL);
		bdev->size = stm32f7_sd_ioctl(bdev, IOCTL_GETDEVSIZE, NULL, 0);
		return 0;
	} else if (BSP_SD_IsDetected() != SD_PRESENT) {
		/* microSD Card is not inserted, do nothing. */
		return 0;
	} else {
		log_error("BSP_SD_Init error");
		return -1;
	}
}

static const struct block_dev_ops stm32f7_sd_driver = {
	.bdo_ioctl = stm32f7_sd_ioctl,
	.bdo_read  = stm32f7_sd_read,
	.bdo_write = stm32f7_sd_write,
	.bdo_probe = stm32f7_sd_init,
};

BLOCK_DEV_DRIVER_DEF(STM32F7_SD_DEVNAME, &stm32f7_sd_driver);
