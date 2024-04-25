/**
 * @file
 *
 * @date Aug 23, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_IN_CHIP_FLASH_H_
#define SRC_DRIVERS_FLASH_IN_CHIP_FLASH_H_

#include <stdint.h>
#include <framework/mod/options.h>

#define IN_CHIP_FLASH_SIZE          OPTION_GET(NUMBER, flash_size)
#define IN_CHIP_FLASH_SECTOR_SIZE   OPTION_GET(NUMBER, flash_sector_size)

#define IN_CHIP_FLASH_DECL() \
    extern char __in_chip_flash_start, __in_chip_flash_end

#define IN_CHIP_FLASH_START ((uintptr_t)&__in_chip_flash_start)
#define IN_CHIP_FLASH_END   ((uintptr_t)&__in_chip_flash_end)

#endif /* SRC_DRIVERS_FLASH_IN_CHIP_FLASH_H_ */
