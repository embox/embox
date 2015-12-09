#include <drivers/block_dev/flash/stm32flash.h>


SECTIONS {
	.flash (NOLOAD): ALIGN(STM32F3_FLASH_PAGE_SIZE) {
		_flash_start = .;
		. += STM32F3_FLASH_FLASH_SIZE;
		_flash_end = .;
	}
}

