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

//#include "stm32_sys_helper.h"

#include <framework/mod/options.h>

#define IMAGE_ADDR   OPTION_GET(NUMBER,image_addr)
#define IMAGE_SIZE   OPTION_GET(NUMBER,image_size)
#define IMAGE_END    (IMAGE_ADDR + IMAGE_SIZE)

extern int libflash_flash_unlock(void);
extern int libflash_flash_lock(void);
extern void libflash_erase_sector(uint32_t sector);
extern void libflash_program_64(uint32_t add, uint64_t data);

__NO_RETURN __STATIC_INLINE void __stm32_system_reset(void) {
  __DSB();                                                          /* Ensure all outstanding memory accesses included
                                                                     buffered write are completed before reset */
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
                           (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                            SCB_AIRCR_SYSRESETREQ_Msk    );         /* Keep priority group unchanged */
  __DSB();                                                          /* Ensure completion of memory access */

  for(;;)                                                        /* wait until reset */
  {
    __NOP();
  }
}

extern char _flash_start, _flash_end;
#define STM32_FLASH_START ((uint32_t)&_flash_start)
#define STM32_FLASH_END   ((uint32_t)&_flash_end)

static inline void flash_write_line(uint32_t wr_addr, uint64_t buf[16]) {
	int i;

	libflash_flash_unlock();
	for (i = 0; i < 16; i++) {
		//libflash_program_64(wr_addr, buf[i]);
		wr_addr += sizeof(buf[0]);
	}
	libflash_flash_lock();
}

static void flasher_copy_blocks(void) {
	uint64_t buf[16];
	uint32_t wr_addr = STM32_ADDR_FLASH_SECTOR_0;
	uint32_t rd_addr = IMAGE_ADDR;
	//uint32_t sec_nr;
	int sec_size;
	int i, j;

	for (; rd_addr < IMAGE_END;) {

		if (wr_addr == STM32_FLASH_START) {
			/* first sector includes stm32_flash0 don't touch it */
			wr_addr += (STM32_FLASH_END - STM32_FLASH_START);
			rd_addr += (STM32_FLASH_END - STM32_FLASH_START);
		}

		//sec_nr = stm32_flash_sector_by_addr(wr_addr);
		sec_size = stm32_flash_sector_size_by_addr(wr_addr);

		libflash_flash_unlock();
		//libflash_erase_sector(sec_nr);
		libflash_flash_lock();

		for (i = 0; i < sec_size; i += sizeof(buf)) {
			for(j = 0; j < sizeof(buf)/ sizeof(buf[0]); j += sizeof(buf[0])) {
				buf[j] = ((volatile uint64_t *)(rd_addr))[j];
			}

			flash_write_line(wr_addr, buf);

			wr_addr += sizeof(buf);
			rd_addr += sizeof(buf);
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

	__NVIC_SystemReset();

	return 0;
}
