/**
 * @file
 *
 * @date Aug 23, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_STM32_IN_CHIP_FLASH_H_
#define SRC_DRIVERS_FLASH_STM32_IN_CHIP_FLASH_H_

#include <framework/mod/options.h>

#define STM32_FLASH_FLASH_SIZE OPTION_GET(NUMBER, flash_size)

#endif /* SRC_DRIVERS_FLASH_STM32_IN_CHIP_FLASH_H_ */
