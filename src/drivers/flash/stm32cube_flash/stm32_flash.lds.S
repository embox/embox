#include <drivers/block_dev/flash/stm32_in_chip_flash.h>

SECTIONS {
	.flash (NOLOAD): ALIGN(IN_CHIP_FLASH_SECTOR_SIZE) {
		_flash_start = .;
		. += IN_CHIP_FLASH_SIZE;
		_flash_end = .;
	}
}
