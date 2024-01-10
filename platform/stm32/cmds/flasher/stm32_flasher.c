/**
 * @file
 *
 * @date   30.10.2020
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <hal/ipl.h>

#include <drivers/block_dev/flash/stm32flash.h>

#include <framework/mod/options.h>

#define DEBUGGING    1

#define IMAGE_ADDR   OPTION_GET(NUMBER,image_addr)
#define IMAGE_SIZE   0x8000
 //OPTION_GET(NUMBER,image_size)
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
#if DEBUGGING != 1
		libflash_program_64(wr_addr, buf[i]);
#else
		printf("wr_addr (%x) val (%" PRIx64 ")\n", wr_addr,  buf[i]);
#endif
		wr_addr += sizeof(buf[0]);
	}
	libflash_flash_lock();
}

static void flasher_copy_blocks(void) {
	uint64_t buf[16];
	uint32_t wr_addr = STM32_ADDR_FLASH_SECTOR_0;
	uint32_t rd_addr = IMAGE_ADDR;
	uint32_t sec_nr;
	int sec_size;
	int i;

	for (; rd_addr < (uint32_t)IMAGE_END;) {

		if (wr_addr == STM32_FLASH_START) {
			/* first sector includes stm32_flash0 don't touch it */
			wr_addr += (STM32_FLASH_END - STM32_FLASH_START);
			rd_addr += (STM32_FLASH_END - STM32_FLASH_START);
		}

		sec_nr = stm32_flash_sector_by_addr(wr_addr);
		if(sec_nr == -1) {
			return;
		}
		sec_size = stm32_flash_sector_size_by_addr(wr_addr);

		libflash_flash_unlock();
#if DEBUGGING != 1
		libflash_erase_sector(sec_nr);
#endif
		libflash_flash_lock();

		//printf("sec (%d) sec size (%x)\n", sec_nr, sec_size);

		for (i = 0; i < sec_size; i += sizeof(buf)) {
			int j;

			for(j = 0; j < (sizeof(buf) / sizeof(buf[0]) ); j ++) {
				//printf("rd_addr (%x) val (%" PRIx64 ")\n", rd_addr, *(volatile uint64_t *)(rd_addr));
				buf[j] = *(volatile uint64_t *)(rd_addr);
				//printf("rd_addr (%x) buf[%d] (%" PRIx64 ")\n", rd_addr, j,  buf[j]);
				rd_addr += sizeof(buf[0]);
			}

			flash_write_line(wr_addr, buf);

			wr_addr += sizeof(buf);	
		//	printf("i (%d) sec size (%d)\n", i,  sec_size);
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