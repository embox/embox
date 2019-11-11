/**
 * @file
 * @brief SD-Card driver for STM32F4-Discovery using SDIO.
 *
 * @author  Denis Deryugin
 * @date    26 Jan 2015
 */

#include <string.h>

#include <drivers/block_dev.h>
#include <framework/mod/options.h>
#include <kernel/time/ktime.h>

#include "stm32f4_discovery_sdio_sd.h"

#include <embox/unit.h>

#include <util/log.h>

static int stm32f4_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size);
static int stm32f4_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);
static int stm32f4_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);

#define STM32F4_SD_DEVNAME "stm32f4_sd_card"
#define SD_BUF_SIZE OPTION_GET(NUMBER, sd_buf_size)

#if OPTION_GET(BOOLEAN, use_local_buf)
#define USE_LOCAL_BUF
#endif

block_dev_driver_t stm32f4_sd_driver = {
	.name  = STM32F4_SD_DEVNAME,
	.ioctl = stm32f4_sd_ioctl,
	.read  = stm32f4_sd_read,
	.write = stm32f4_sd_write,
	.probe = NULL,
};

EMBOX_UNIT_INIT(stm32f4_sd_init);

static int stm32f4_sd_init(void) {
	struct block_dev *bdev;

	if (BSP_SD_Init() == MSD_OK) {
		log_debug("SD card present");
		bdev = block_dev_create(STM32F4_SD_DEVNAME, &stm32f4_sd_driver, NULL);
		assert(bdev);

		bdev->size = (unsigned) stm32f4_sd_ioctl(bdev, IOCTL_GETDEVSIZE, NULL, 0);
	} else {
		log_debug("SD card is not present");
	}

	return 0;
}

static int stm32f4_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size) {
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
static int stm32f4_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	assert(count <= SD_BUF_SIZE);
	int res;
	size_t bsize = bdev->block_size;
#ifdef USE_LOCAL_BUF
	res = BSP_SD_ReadBlocks((uint32_t *) sd_buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	while (BSP_SD_GetStatus() != SD_TRANSFER_OK);
	memcpy(buf, sd_buf, bsize);
#else
	res = BSP_SD_ReadBlocks((uint32_t *) buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	while (BSP_SD_GetStatus() != SD_TRANSFER_OK);
#endif

	return res;
}

static int stm32f4_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	assert(count <= SD_BUF_SIZE);
	int res;
	size_t bsize = bdev->block_size;

#ifdef USE_LOCAL_BUF
	memcpy(sd_buf, buf, bsize);
	res = BSP_SD_WriteBlocks((uint32_t *) sd_buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	while (BSP_SD_GetStatus() != SD_TRANSFER_OK);
#else
	res = BSP_SD_WriteBlocks((uint32_t *) buf, blkno * bsize, bsize, 1) ? -1 : bsize;
	while (BSP_SD_GetStatus() != SD_TRANSFER_OK);
#endif
	return res;
}
