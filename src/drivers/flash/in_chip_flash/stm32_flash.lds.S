#include <drivers/block_dev/flash/stm32_in_chip_flash.h>

SECTIONS {
	.inchip_flash_section (NOLOAD): ALIGN(IN_CHIP_FLASH_SECTOR_SIZE) {
		__in_chip_flash_start = .;
		. += IN_CHIP_FLASH_SIZE;
		__in_chip_flash_end = .;
	}
}
