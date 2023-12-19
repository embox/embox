/**
 * @file
 *
 * @date Aug 23, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_STM32_STM32F2FLASH_H_
#define SRC_DRIVERS_FLASH_STM32_STM32F2FLASH_H_

#define STM32_ADDR_FLASH_SECTOR_0      ((uint32_t)0x08000000)
/* First 4 sectors of STM32F4-Discovery flash are 16Kb */
#define STM32_FLASH_SECTOR_SIZE        (16 * 1024)
/* We use only first 4 16Kb sectors */
#define STM32_FLASH_SECTORS_COUNT      4

#define STM32_FLASH_WORD               (4)

#ifndef __ASSEMBLER__
#include <stm32f2xx_hal.h>
#include <string.h>

static inline void stm32_fill_flash_erase_struct(
		FLASH_EraseInitTypeDef *erase_struct,
		unsigned int block) {
	memset(erase_struct, 0, sizeof *erase_struct);
	erase_struct->TypeErase = FLASH_TYPEERASE_SECTORS;
	erase_struct->Sector    = block;
	erase_struct->NbSectors = 1;
}
#endif

#endif /* SRC_DRIVERS_FLASH_STM32_STM32F2FLASH_H_ */
