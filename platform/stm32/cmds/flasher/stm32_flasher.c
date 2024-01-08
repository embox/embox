/**
 * @file
 *
 * @date   30.10.2020
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <stdio.h>

#include <hal/ipl.h>

#include <drivers/block_dev/flash/stm32flash.h>

#include <framework/mod/options.h>

#define IMAGE_ADDR   OPTION_GET(NUMBER,image_addr)

extern int libflash_flash_unlock(void);
extern int libflash_flash_lock(void);
extern void libflash_erase_sector(uint32_t sector);
extern void libflash_program_64(uint32_t add, uint64_t data);

extern char _flash_start, _flash_end;
#define STM32_FLASH_START ((uint32_t)&_flash_start)
#define STM32_FLASH_END   ((uint32_t)&_flash_end)

static void flasher_copy_blocks(void) {
	uint32_t sector = stm32_flash_sector_by_addr(IMAGE_ADDR);

	libflash_flash_unlock();
	libflash_erase_sector(sector);
	libflash_flash_lock();
}

int main(int argc, char **argv) {

	printf("addr flasher(%p)\n", flasher_copy_blocks);

	ipl_disable();
	{
		flasher_copy_blocks();
	}
	ipl_enable();

	return 0;
}
