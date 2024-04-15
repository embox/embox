/**
 * @file
 *
 * @date 09.12.2015
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_STM32H7FLASH_H_
#define SRC_DRIVERS_FLASH_STM32H7FLASH_H_

#define STM32_ADDR_FLASH_SECTOR_0 ((uint32_t)0x08000000)
#define STM32_FLASH_SECTOR_SIZE   (128 * 1024)
#define STM32_FLASH_SECTORS_COUNT 4
#define STM32_FLASH_WORD          (32)

#ifndef __ASSEMBLER__
#include <stm32h7xx_hal.h>
#include <string.h>
#define FLASH_TYPEPROGRAM_WORD FLASH_TYPEPROGRAM_FLASHWORD

static inline void stm32_fill_flash_erase_struct(
    FLASH_EraseInitTypeDef *erase_struct, unsigned int block) {
	memset(erase_struct, 0, sizeof *erase_struct);
	erase_struct->TypeErase = FLASH_TYPEERASE_SECTORS;
	erase_struct->VoltageRange = FLASH_VOLTAGE_RANGE_3;
	erase_struct->Banks = FLASH_BANK_1;
	erase_struct->Sector = block;
	erase_struct->NbSectors = 1;
}
#endif

#endif /* SRC_DRIVERS_FLASH_STM32H7FLASH_H_ */
