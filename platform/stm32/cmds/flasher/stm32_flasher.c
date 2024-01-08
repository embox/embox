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
	uint64_t buf[16];
	uint32_t wr_addr = IMAGE_ADDR;//STM32_ADDR_FLASH_SECTOR_0;
	uint32_t rd_addr = IMAGE_ADDR;
	uint32_t sec_nr;
	int sec_size;
	int i, j;

	/* first sector includes stm32_flash0 don't touch it */
	sec_nr = stm32_flash_sector_by_addr(wr_addr);
	sec_size = stm32_flash_sector_size_by_addr(wr_addr);
	libflash_flash_unlock();
	libflash_erase_sector(sec_nr);
	libflash_flash_lock();
	for (i = 0; i < sec_size; i += sizeof(buf)) {
		for(j = 0; j < sizeof(buf)/ sizeof(buf[0]); j += sizeof(buf[0])) {
			buf[j] = *( volatile uint64_t *)(rd_addr + j);
		}
	}

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
