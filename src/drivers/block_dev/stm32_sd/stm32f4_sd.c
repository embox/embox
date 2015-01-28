/**
 * @file
 * @brief SD-Card driver for STM32F4-Discovery using SDIO.
 *
 * @author  Denis Deryugin
 * @date    26 Jan 2015
 */

#include <embox/block_dev.h>
#include "stm32f4_discovery_sdio_sd.h"

static int stm32f4_sd_init(void *arg);
static int stm32f4_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size);
static int stm32f4_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);
static int stm32f4_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);

#define STM32F4_SD_DEVNAME "stm32f4_sd_card"

block_dev_driver_t stm32f4_sd_driver = {
	.name = STM32F4_SD_DEVNAME,
	.ioctl = stm32f4_sd_ioctl,
	.read = stm32f4_sd_read,
	.write = stm32f4_sd_write,
};

EMBOX_BLOCK_DEV(STM32F4_SD_DEVNAME, &stm32f4_sd_driver, stm32f4_sd_init);

static int stm32f4_sd_init(void *arg) {
	return -SD_Init();
}

static int stm32f4_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size) {
	return 0;
}

static int stm32f4_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	return -SD_ReadBlock((uint8_t*) buf, blkno, SECTOR_SIZE);
}

static int stm32f4_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	return -SD_WriteBlock((uint8_t*) buf, blkno, SECTOR_SIZE);
}

