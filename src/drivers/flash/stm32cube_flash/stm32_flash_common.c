/**
 * @file
 *
 * @brief Flash driver for stm32 F3/F4/F7
 *
 * @details F4 and F7 have sectors of different sizes.
 *   For example, STM32F4-Discovery:
 *      - 4 16Kb sectors
 *      - 1 64Kb sector
 *      - 7 128Kb sector
 *   So we use only first 4 64Kb sectors.
 *   For example, STM32F3-Discovery:
 *      - 256 2Kb sectors (they are called pages)
 *   So we use only first 256 2Kb sectors.
 *
 * @author  Anton Kozlov
 * @author  Alexander Kalmuk
 *             Fixes and adoptation to new STM32Cube
 * @date    23.10.2014
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <drivers/block_dev/flash/stm32flash.h>
#include <drivers/flash/flash.h>
#include <util/log.h>
#include <util/macro.h>
#include <util/math.h>

static inline int stm32_flash_check_word_aligned(unsigned long base,
    size_t len) {
	return ((uintptr_t)base & (STM32_FLASH_WORD - 1)) == 0
	       && ((uintptr_t)len & (STM32_FLASH_WORD - 1)) == 0;
}


#if defined(STM32_FLASH_VAR_BLOCK_SIZE) && (STM32_FLASH_VAR_BLOCK_SIZE == 1)
static inline int stm32_flash_get_start_sector(struct flash_dev *dev) {
	return stm32_flash_sector_by_addr(dev->block_info[0].fbi_start_id);
}
#else
static inline int stm32_flash_get_start_sector(struct flash_dev *dev) {
	return ((dev->block_info[0].fbi_start_id - STM32_ADDR_FLASH_SECTOR_0)
	        / dev->block_info[0].block_size);
}
#endif /* defined(STM32_FLASH_VAR_BLOCK_SIZE) */

int stm32_flash_erase_block(struct flash_dev *dev, uint32_t block) {
	int ret = 0;
	int repeat = 3;
	uint32_t page_err;
	FLASH_EraseInitTypeDef erase_struct;

	assert(block < dev->block_info[0].blocks);
	assert(dev->num_block_infos == 1);

	/* block is relative to flash beginning with not
	 * the actual ROM start address. So calculate the new sector
	 * in terms of ROM start address. */
	block += stm32_flash_get_start_sector(dev);
	log_debug("Erase global block %d", block);

	stm32_fill_flash_erase_struct(&erase_struct, block);

	while (repeat--) {
		HAL_FLASH_Unlock();
		ret = HAL_FLASHEx_Erase(&erase_struct, &page_err);
		HAL_FLASH_Lock();
		if (ret == HAL_OK) {
			break;
		}
	}
	if (ret != HAL_OK) {
		log_error("Failed to erase block %i", block);
	}

	return ret;
}

int stm32_flash_read(struct flash_dev *dev, uint32_t base, void *data,
    size_t len) {
	if (base + len > dev->size) {
		log_error("Address is out of range. Base=0x%x,len=0x%x", base, len);
		return -1;
	}
	/* read can be unaligned */
	memcpy(data, (void *)(dev->block_info[0].fbi_start_id) + base, len);

	return len;
}

int stm32_flash_program(struct flash_dev *dev, uint32_t base, const void *data,
    size_t len) {
	uint32_t *data32;
	uint32_t flash_err;
	uint32_t dest;
	int rep;
	int err;
	int i;

	err = -1;

	/* Check alignment to uint32_t boundary */
	if (!stm32_flash_check_word_aligned(base, len)
	    || ((uintptr_t)data & (sizeof(*data32) - 1)) != 0) {
		err = -EINVAL;
		goto err_exit;
	}

	if (base + len > dev->size) {
		log_error("Address is out of range. Base=0x%x,len=0x%x", base, len);
		err = -EFBIG;
		goto err_exit;
	}

	/* Copy by word */
	dest = dev->block_info[0].fbi_start_id + base;
	data32 = (uint32_t *)data;

	HAL_FLASH_Unlock();
	for (i = 0; i < len / sizeof(*data32);
	     i += (STM32_FLASH_WORD / sizeof(*data32))) {
		for (rep = 3; rep >= 0; rep--) {
#if defined(STM32H7_CUBE)
			flash_err = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dest,
			    (uint32_t)&data32[i]);
#elif defined(STM32L475xx)
			flash_err = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, dest,
			    *(uint64_t *)&data32[i]);
#else
			assert(STM32_FLASH_WORD == 4);
			flash_err = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dest,
			    data32[i]);
#endif
			if (flash_err != HAL_OK) {
				flash_err = HAL_FLASH_GetError();
				log_error("dest=0x%08x,len=0x%x, flash_err 0x%x", dest, len,
				    flash_err);
				err = -EBUSY;
				continue;
			}
			else {
				err = 0;
				break;
			}
		}
		if (err) {
			HAL_FLASH_Lock();
			goto err_exit;
		}

		dest += STM32_FLASH_WORD;
	}
	HAL_FLASH_Lock();
	return len;

err_exit:
	log_error("base=0x%08x,data=%p,len=0x%x", base, data, len);
	return err;
}
