/**
 * @file
 *
 * @data 0912.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_STM32_FLASH_H_
#define SRC_DRIVERS_FLASH_STM32_FLASH_H_

#include <framework/mod/options.h>

#define STM32F3_FLASH_PAGE_SIZE     OPTION_GET(NUMBER,page_size)
#define STM32F3_FLASH_FLASH_SIZE    OPTION_GET(NUMBER,flash_size)

#endif /* SRC_DRIVERS_FLASH_STM32_FLASH_H_ */
