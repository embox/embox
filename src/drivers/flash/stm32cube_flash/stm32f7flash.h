/**
 * @file
 *
 * @date 09.12.2015
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_STM32F7FLASH_H_
#define SRC_DRIVERS_FLASH_STM32F7FLASH_H_


/* First 4 sectors of STM32F4-Discovery flash are 32Kb */
#define STM32_FLASH_SECTOR_SIZE   (32 * 1024)
/* We use only first 4 32Kb sectors */
#define STM32_FLASH_SECTORS_COUNT 4



#define STM32_ADDR_FLASH_SECTOR_0 ((uint32_t)0x08000000)

#define STM32_FLASH_WORD              (4)

#define STM32_FLASH_VAR_BLOCK_SIZE    1

#undef DUAL_BANK

/* Base address of the Flash sectors */
#if defined(DUAL_BANK)
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base address of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base address of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base address of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base address of Sector 11, 128 Kbytes */
#define ADDR_FLASH_SECTOR_12    ((uint32_t)0x08100000) /* Base address of Sector 12, 16 Kbytes */
#define ADDR_FLASH_SECTOR_13    ((uint32_t)0x08104000) /* Base address of Sector 13, 16 Kbytes */
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x08108000) /* Base address of Sector 14, 16 Kbytes */
#define ADDR_FLASH_SECTOR_15    ((uint32_t)0x0810C000) /* Base address of Sector 15, 16 Kbytes */
#define ADDR_FLASH_SECTOR_16    ((uint32_t)0x08110000) /* Base address of Sector 16, 64 Kbytes */
#define ADDR_FLASH_SECTOR_17    ((uint32_t)0x08120000) /* Base address of Sector 17, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18    ((uint32_t)0x08140000) /* Base address of Sector 18, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19    ((uint32_t)0x08160000) /* Base address of Sector 19, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20    ((uint32_t)0x08180000) /* Base address of Sector 20, 128 Kbytes */
#define ADDR_FLASH_SECTOR_21    ((uint32_t)0x081A0000) /* Base address of Sector 21, 128 Kbytes */
#define ADDR_FLASH_SECTOR_22    ((uint32_t)0x081C0000) /* Base address of Sector 22, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23    ((uint32_t)0x081E0000) /* Base address of Sector 23, 128 Kbytes */
#else
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 32 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08008000) /* Base address of Sector 1, 32 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08010000) /* Base address of Sector 2, 32 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08018000) /* Base address of Sector 3, 32 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08020000) /* Base address of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) /* Base address of Sector 5, 256 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08080000) /* Base address of Sector 6, 256 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x080C0000) /* Base address of Sector 7, 256 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08100000) /* Base address of Sector 8, 256 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x08140000) /* Base address of Sector 9, 256 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x08180000) /* Base address of Sector 10, 256 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x081C0000) /* Base address of Sector 11, 256 Kbytes */
#endif /* DUAL_BANK */

#ifndef __ASSEMBLER__
#include <stdint.h>

static inline int stm32_flash_sector_by_addr(uint32_t addr) {
	switch(addr) {
		case ADDR_FLASH_SECTOR_0:
			return 0;
		case ADDR_FLASH_SECTOR_1:
			return 1;
		case ADDR_FLASH_SECTOR_2:
			return 3;
		case ADDR_FLASH_SECTOR_3:
			return 4;
		case ADDR_FLASH_SECTOR_4:
			return 5;
		case ADDR_FLASH_SECTOR_5:
			return 6;
		case ADDR_FLASH_SECTOR_6:
			return 7;
		case ADDR_FLASH_SECTOR_7:
			return 8;
		case ADDR_FLASH_SECTOR_8:
			return 9;
		case ADDR_FLASH_SECTOR_9:
			return 10;
		case ADDR_FLASH_SECTOR_10:
			return 11;
		case ADDR_FLASH_SECTOR_11:
			return 0;
		default:
			return -1;
	}
	return -1;
}
#endif

#ifndef __ASSEMBLER__
#include <stm32f7xx_hal.h>
#include <string.h>

static inline void stm32_fill_flash_erase_struct(
    FLASH_EraseInitTypeDef *erase_struct, unsigned int block) {
	memset(erase_struct, 0, sizeof *erase_struct);
	erase_struct->TypeErase = FLASH_TYPEERASE_SECTORS;
	erase_struct->VoltageRange = FLASH_VOLTAGE_RANGE_3;
	erase_struct->Sector = block;
	erase_struct->NbSectors = 1;
}
#endif

#endif /* SRC_DRIVERS_FLASH_STM32F7FLASH_H_ */
