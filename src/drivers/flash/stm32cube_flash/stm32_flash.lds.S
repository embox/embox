#include <drivers/block_dev/flash/stm32flash.h>
#include <drivers/block_dev/flash/stm32_in_chip_flash.h>

SECTIONS {
	.flash (NOLOAD): ALIGN(STM32_FLASH_SECTOR_SIZE) {
		_flash_start = .;
		. += STM32_FLASH_FLASH_SIZE;
		_flash_end = .;
	}
}
