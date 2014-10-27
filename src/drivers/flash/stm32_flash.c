/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.10.2014
 */

#include <assert.h>
#include <errno.h>
#include <hal/reg.h>
#include <hal/mem.h>
#include <drivers/flash/flash_dev.h>
#include <stm32f4xx_flash.h>

#define FLASH_MEMORY_START 0x08000000

#if 0
static const unsigned stm32_flash_g_sector_sizes[] = {
	0x4000,
	0x4000,
	0x4000,
	0x4000,
	0x10000,
	0x20000,
	0x20000,
	0x20000,
	0x20000,
	0x20000,
	0x20000,
	0x20000,
};
#endif

static inline void stm32_flash_set_program_size(void) {
	/* setting word 32-bit access */
	REG_STORE(&FLASH->CR, (REG_LOAD(&FLASH->CR) & CR_PSIZE_MASK) | FLASH_PSIZE_WORD);
}

static int stm32_flash_erase_block(struct flash_dev *dev, uint32_t block) {
	FLASH_Status status;
	int err;

	if (block >= 12) {
		return -EINVAL;
	}

	FLASH_Unlock();

	err = 0;
	status = FLASH_WaitForLastOperation();
	if (status == FLASH_COMPLETE) {

		stm32_flash_set_program_size();

		REG_STORE(&FLASH->CR, (REG_LOAD(&FLASH->CR) & 0xffffff07) | FLASH_CR_SER | (block << 3));

		REG_ORIN(&FLASH->CR, FLASH_CR_STRT);

		FLASH_WaitForLastOperation();

		REG_ANDIN(&FLASH->CR, ~FLASH_CR_SER);
	} else {
		err = -EIO;
	}

	FLASH_Lock();

	return err;
}

static int stm32_flash_program(struct flash_dev *dev, uint32_t base, const void* data, size_t len) {
	FLASH_Status status;
	int err;

	if (((uintptr_t) base & 0x2) != 0
			|| ((uintptr_t) data & 0x2) != 0
			|| ((uintptr_t) len  & 0x2) != 0) {
		return -EINVAL;
	}

	FLASH_Unlock();

	err = 0;
	status = FLASH_WaitForLastOperation();
	if (status == FLASH_COMPLETE) {

		stm32_flash_set_program_size();

		REG_ORIN(&FLASH->CR, FLASH_CR_PG);

		regcpy32((void *) FLASH_MEMORY_START + base, data, len >> 2);

		FLASH_WaitForLastOperation();

		REG_ANDIN(&FLASH->CR, ~FLASH_CR_PG);
	} else {
		err = -EIO;
	}

	FLASH_Lock();

	return err;
}

#if 0
static int stm32_flash_init(void *arg);
static size_t stm32_flash_query(struct flash_dev *dev, void * data, size_t len);
static int flash_read(struct flash_dev *dev, uint32_t base, void* data, size_t len);
#endif
struct flash_dev_drv stm32_flash_drv = {
#if 0
	.flash_init = stm32_flash_init,
	.flash_query = stm32_flash_query,
	.flash_read = stm32_flash_read,
#endif
	.flash_erase_block = stm32_flash_erase_block,
	.flash_program = stm32_flash_program,
};
