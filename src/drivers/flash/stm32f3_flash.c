/**
 * @file
 * @brief Flash driver for STM32F3Discovery board
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-08
 */

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <util/math.h>

#include <drivers/block_dev/flash/flash_dev.h>
#include <fs/node.h>
#include <hal/reg.h>
#include <hal/mem.h>

#include <stm32f3xx_hal.h>

#define STM32F3_FIRST_BLOCK_OFFSET 1
#define STM32F3_FLASH_PAGE_SIZE ((uint32_t) 0x800)
#define STM32F3_ERR_MASK 0x1f3

static const struct flash_dev_drv stm32f3_flash_drv;
const struct flash_dev stm32f3_flash = {
	.bdev = NULL,
	.drv = &stm32f3_flash_drv,
	.flags = 0,
	.start = 0x08006000,
	.end   = 0x08007000,
	.num_block_infos = 1,
	.block_info = {
		.block_size = 0x000800,
		.blocks = 4
	},
};

static inline int stm32f3_flash_check_range(struct flash_dev *dev, unsigned long base, size_t len) {
	return dev->start + base + len <= dev->end;
}

static inline int stm32f3_flash_check_align(unsigned long base, size_t len) {
	return ((uintptr_t) base & 0x3) == 0 && ((uintptr_t) len  & 0x3) == 0;
}

static inline int stm32f3_flash_check_block(struct flash_dev *dev, uint32_t block) {
	unsigned int n_block, i;

	n_block = 0;
	for (i = 0; i < dev->num_block_infos; i ++) {
		n_block += dev->block_info.blocks;
	}

	return block < n_block;
}


/**
 * @brief Erase flash block
 * @note Block is the same thing as page in terms of stm32 lib:
 * it's minimal erasable chunk of flash memory
 *
 * @param dev
 * @param block
 *
 * @return Error code
 */
static int stm32f3_flash_erase_block(struct flash_dev *dev, uint32_t block) {
	int err;
	uint32_t page_err;
	FLASH_EraseInitTypeDef erase_struct;

	if (!stm32f3_flash_check_block(dev, block)) {
		return -EINVAL;
	}

	HAL_FLASH_Unlock();

	erase_struct = (FLASH_EraseInitTypeDef) {
		.TypeErase = TYPEERASE_PAGES,
		.PageAddress = dev->start + block * STM32F3_FLASH_PAGE_SIZE,
		.NbPages = 1,
	};

	err = HAL_FLASHEx_Erase(&erase_struct, &page_err);
	/* TODO check errcode */
	/* Lock to prevent unwanted operations with flash memory */
	HAL_FLASH_Lock();

	return err;
}


static int stm32f3_flash_read(struct flash_dev *dev, uint32_t base, void* data, size_t len) {
	size_t rlen = min(len, dev->end - dev->start + base);

	/* read can be unaligned */

	memcpy(data, (void *) dev->start + base, rlen);

	return rlen;
}

/**
 * @brief Write data to flash memory
 *
 * @param dev
 * @param base
 * @param data
 * @param len
 *
 * @return Negative error code
 */
static int stm32f3_flash_program(struct flash_dev *dev, uint32_t base, const void* data, size_t len) {
	int err = 0;
	int offset;

	if (!stm32f3_flash_check_align(base, len)
			|| ((uintptr_t) data & 0x3) != 0) {
		return -EINVAL;
	}

	if (!stm32f3_flash_check_range(dev, base, len)) {
		return -EFBIG;
	}

	HAL_FLASH_Unlock();

	/* Write data word by word */
	for (offset = 0; offset < len; offset += 4) {
		if (!HAL_FLASH_Program(TYPEPROGRAM_WORD,
					dev->start + base + offset,
					*((uint32_t*) data + offset)))
			return -1; /* TODO: set appropriate code */
	}

	/* Lock to prevent unwanted operations with flash memory */
	HAL_FLASH_Lock();

	return err;
}

static int stm32f3_flash_copy(struct flash_dev *dev, uint32_t base_dst,
				uint32_t base_src, size_t len) {
	return stm32f3_flash_program(dev, base_dst, (void *) dev->start + base_src, len);
}

static const struct flash_dev_drv stm32f3_flash_drv = {
	.flash_read = stm32f3_flash_read,
	.flash_erase_block = stm32f3_flash_erase_block,
	.flash_program = stm32f3_flash_program,
	.flash_copy = stm32f3_flash_copy,
};
