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

#include <framework/mod/options.h>

#define NAND_PAGE_SIZE         OPTION_GET(NUMBER, page_size)
#define NAND_BLOCK_SIZE        OPTION_GET(NUMBER, block_size)
#define USE_RAM_AS_CACHE       OPTION_GET(BOOLEAN, use_ram_as_cache)
#define USE_RAM_SECTION        OPTION_GET(BOOLEAN, use_ram_section)


#if USE_RAM_SECTION
#define CACHE_SECTION      __attribute__((section(".dfs_cache_section")))
#else
#define CACHE_SECTION
#endif

static uint8_t cache_block_buffer[NAND_BLOCK_SIZE]
								  CACHE_SECTION  __attribute__ ((aligned(NAND_PAGE_SIZE)));

uintptr_t flash_cache_addr(struct flash_dev *flashdev) {
	return (uintptr_t) cache_block_buffer;
}

