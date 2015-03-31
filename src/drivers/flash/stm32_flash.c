/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.10.2014
 */

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <hal/reg.h>
#include <hal/mem.h>
#include <kernel/printk.h>
#include <util/math.h>
#include <drivers/block_dev/flash/flash_dev.h>
#include <stm32f4xx_flash.h>

#define STM32_FIRST_BLOCK_OFFSET 1
#define STM32_ERR_MASK 0x1f3

static const struct flash_dev_drv stm32_flash_drv;
const struct flash_dev stm32_flash = {
	.bdev = NULL,
	.drv = &stm32_flash_drv,
	.flags = 0,
	.start = 0x08004000,
	.end   = 0x08010000,
	.num_block_infos = 1,
	.block_info = {
		.block_size = 0x004000,
		.blocks = 3
	},
};

static inline void stm32_flash_set_program_size(void) {
	/* setting word 32-bit access */
	REG_STORE(&FLASH->CR, (REG_LOAD(&FLASH->CR) & CR_PSIZE_MASK) | FLASH_PSIZE_WORD);
}

static inline int stm32_flash_check_range(struct flash_dev *dev, unsigned long base, size_t len) {
	return dev->start + base + len <= dev->end;
}

static inline int stm32_flash_check_align(unsigned long base, size_t len) {
	return ((uintptr_t) base & 0x3) == 0 && ((uintptr_t) len  & 0x3) == 0;
}
static inline void stm32_flash_wait(void) {
	while (REG_LOAD(&FLASH->SR) & FLASH_FLAG_BSY) {
		/* nop */
	}
}

static inline int stm32_flash_check_block(struct flash_dev *dev, uint32_t block) {
	unsigned int n_block, i;

	n_block = 0;
	for (i = 0; i < dev->num_block_infos; i ++) {
		n_block += dev->block_info.blocks;
	}

	return block < n_block;
}

static int stm32_flash_erase_block(struct flash_dev *dev, uint32_t block) {
	unsigned long sr;
	int err;

	if (!stm32_flash_check_block(dev, block)) {
		return -EINVAL;
	}

	FLASH_Unlock();

	stm32_flash_wait();

	/* clear all errors */
	REG_STORE(&FLASH->SR, STM32_ERR_MASK);

	stm32_flash_set_program_size();

	REG_STORE(&FLASH->CR,
			(REG_LOAD(&FLASH->CR) & 0xffffff07)
			| FLASH_CR_SER
			| ((STM32_FIRST_BLOCK_OFFSET + block) << 3));

	sr = REG_LOAD(&FLASH->SR);
	if (!(sr & STM32_ERR_MASK)) {
		REG_ORIN(&FLASH->CR, FLASH_CR_STRT);
		stm32_flash_wait();
		err = 0;
	} else {
		printk("%s: EIO sr=%08lx\n", __func__, sr);
		err = -EIO;
	}

	FLASH_Lock();

	return err;
}


static int stm32_flash_read(struct flash_dev *dev, uint32_t base, void* data, size_t len) {
	size_t rlen = min(len, dev->end - dev->start + base);

	/* read can be unaligned */

	memcpy(data, (void *) dev->start + base, rlen);

	return rlen;
}

static int stm32_flash_program(struct flash_dev *dev, uint32_t base, const void* data, size_t len) {
	unsigned long sr;
	int err;

	if (!stm32_flash_check_align(base, len)
			|| ((uintptr_t) data & 0x3) != 0) {
		return -EINVAL;
	}

	if (!stm32_flash_check_range(dev, base, len)) {
		return -EFBIG;
	}

	FLASH_Unlock();

	stm32_flash_wait();

	/* clear all errors */
	REG_STORE(&FLASH->SR, STM32_ERR_MASK);

	stm32_flash_set_program_size();

	REG_ORIN(&FLASH->CR, FLASH_CR_PG);

	sr = REG_LOAD(&FLASH->SR);
	if (!(sr & STM32_ERR_MASK)) {
		regcpy32((void *) dev->start + base, data, len >> 2);
		stm32_flash_wait();
		err = 0;
	} else {
		printk("%s: EIO sr=%08lx\n", __func__, sr);
		err = -EIO;
	}

	FLASH_Lock();

	return err;
}

static int stm32_flash_copy(struct flash_dev *dev, uint32_t base_dst,
				uint32_t base_src, size_t len) {
	return stm32_flash_program(dev, base_dst, (void *) dev->start + base_src, len);
}

#if 0
static int stm32_flash_init(void *arg);
static size_t stm32_flash_query(struct flash_dev *dev, void * data, size_t len);
#endif
static const struct flash_dev_drv stm32_flash_drv = {
	.flash_read = stm32_flash_read,
	.flash_erase_block = stm32_flash_erase_block,
	.flash_program = stm32_flash_program,
	.flash_copy = stm32_flash_copy,
};

