/**
 * @file
 *
 * @author Anton Bondarev
 * @date   19 Oct 2023
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <util/math.h>

#include <drivers/flash/flash.h>
#include <drivers/flash/flash_cache.h>

#include <framework/mod/options.h>

#define NAND_PAGE_SIZE         OPTION_GET(NUMBER, page_size)
#define NAND_BLOCK_SIZE        OPTION_GET(NUMBER, block_size)


static uint8_t cache_block_buffer[NAND_BLOCK_SIZE]
								  FLASH_CACHE_SECTION  __attribute__ ((aligned(NAND_PAGE_SIZE)));

uintptr_t flash_cache_addr(struct flash_dev *flashdev) {
	return (uintptr_t) cache_block_buffer;
}

