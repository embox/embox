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

static int stm32f4_sd_init(void *arg);
static int stm32f4_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size);
static int stm32f4_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);
static int stm32f4_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);

#define STM32F4_SD_DEVNAME "stm32f4_sd_card"
#define SD_BUF_SIZE OPTION_GET(NUMBER, sd_buf_size)

block_dev_driver_t stm32f4_sd_driver = {
	.name = STM32F4_SD_DEVNAME,
	.ioctl = stm32f4_sd_ioctl,
	.read = stm32f4_sd_read,
	.write = stm32f4_sd_write,
};

EMBOX_BLOCK_DEV(STM32F4_SD_DEVNAME, &stm32f4_sd_driver, stm32f4_sd_init);

static int stm32f4_sd_init(void *arg) {
	block_dev_t *bdev;
	if (block_dev_lookup(STM32F4_SD_DEVNAME) && (SD_Init() == SD_OK)) {
		bdev = block_dev_create_common(STM32F4_SD_DEVNAME, &stm32f4_sd_driver, NULL);
		bdev->size = stm32f4_sd_ioctl(bdev, IOCTL_GETDEVSIZE, NULL, 0);
		return 0;
	} else {
		return -1;
	}
}

static int stm32f4_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size) {
	SD_CardInfo info;
	int res = SD_GetCardInfo(&info);

	if (res != SD_OK) {
		return -1;
	}

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return info.CardCapacity;
	case IOCTL_GETBLKSIZE:
		return info.CardBlockSize;
	default:
		return -1;
	}
}

static uint8_t sd_buf[SD_BUF_SIZE];

static int stm32f4_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	assert(count <= SD_BUF_SIZE);
	int res;
	size_t bsize = bdev->block_size;
	res = SD_ReadBlock((uint8_t*) sd_buf, blkno * bsize, bsize) ? -1 : bsize;
	while (SD_GetStatus() != SD_TRANSFER_OK);

	memcpy(buf, sd_buf, bsize);
	return res;
}

static int stm32f4_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	assert(count <= SD_BUF_SIZE);
	int res;
	size_t bsize = bdev->block_size;
	memcpy(sd_buf, buf, bsize);
	res = SD_WriteBlock((uint8_t*) sd_buf, blkno * bsize, bsize) ? -1 : bsize;
	while (SD_GetStatus() != SD_TRANSFER_OK);
	return res;
}

