#include <drivers/block_dev/flash/stm32flash_conf.h>


SECTIONS {
	.flash (NOLOAD): ALIGN(STM32_FLASH_PAGE_SIZE) {
		_flash_start = .;
		. += STM32_FLASH_FLASH_SIZE;
		_flash_end = .;
	}
}
