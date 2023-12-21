/**
 * @file
 *
 * @data 09.12.2015
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_STM32L4FLASH_H_
#define SRC_DRIVERS_FLASH_STM32L4FLASH_H_



#define STM32_ADDR_FLASH_SECTOR_0	((uint32_t)0x08000000)
#define STM32_FLASH_SECTOR_SIZE		(2 * 1024)
#define STM32_FLASH_SECTORS_COUNT	512
#define STM32_FLASH_WORD         	(8)

#ifndef __ASSEMBLER__
#include <stm32l4xx_hal.h>
#include <string.h>

static inline void stm32_fill_flash_erase_struct(
		FLASH_EraseInitTypeDef *erase_struct,
		unsigned int block) {
	memset(erase_struct, 0, sizeof *erase_struct);
	erase_struct->TypeErase	= FLASH_TYPEERASE_PAGES;
	erase_struct->Banks	= 1 << (block >> 8);
	erase_struct->Page	= block & 0xFF;
	erase_struct->NbPages	= 1;
}
#endif

#endif /* SRC_DRIVERS_FLASH_STM32L4FLASH_H_ */
