/**
 * @file
 *
 * @data 09.12.2015
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_FLASH_STM32FLASH_CONF_H_
#define SRC_DRIVERS_FLASH_STM32FLASH_CONF_H_

#include <framework/mod/options.h>

#define STM32_FLASH_PAGE_SIZE     OPTION_GET(NUMBER,page_size)
#define STM32_FLASH_FLASH_SIZE    OPTION_GET(NUMBER,flash_size)

#define STM32_FLASH_WORD_ACCESS   OPTION_GET(NUMBER,word_size)
#define STM32_FLASH_WORD_ALIGN    (STM32_FLASH_WORD_ACCESS - 1)

#endif /* SRC_DRIVERS_FLASH_STM32FLASH_CONF_H_ */
