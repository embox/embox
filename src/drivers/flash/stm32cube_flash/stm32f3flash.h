/**
 * @file
 *
 * @data 09.12.2015
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_STM32F3FLASH_H_
#define SRC_DRIVERS_FLASH_STM32F3FLASH_H_

#define STM32_ADDR_FLASH_SECTOR_0      ((uint32_t)0x08000000)
/* All sectors in STM32F3-Discovery (except the last Information block)
 * are 2Kb in size */
#define STM32_FLASH_SECTOR_SIZE        (2 * 1024)
/* We use first 256 2Kb sectors */
#define STM32_FLASH_SECTORS_COUNT      256

#define STM32_FLASH_WORD               (4)

#ifndef __ASSEMBLER__
#include <stm32f3xx_hal.h>
#include <string.h>

static inline void stm32_fill_flash_erase_struct(
		FLASH_EraseInitTypeDef *erase_struct,
		unsigned int block) {
	memset(erase_struct, 0, sizeof *erase_struct);
	erase_struct->TypeErase   = FLASH_TYPEERASE_PAGES;
	erase_struct->PageAddress = STM32_ADDR_FLASH_SECTOR_0 +
		(STM32_FLASH_SECTOR_SIZE * block);
	erase_struct->NbPages     = 1;
}
#endif

#endif /* SRC_DRIVERS_FLASH_STM32F3FLASH_H_ */
