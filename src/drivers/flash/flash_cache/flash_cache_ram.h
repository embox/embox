/**
 * @file
 *
 * @author Denis Deryugin
 * @date 26 Dec 2014
 * 
 */

#ifndef _FLASH_CACHE_RAM_H_
#define _FLASH_CACHE_RAM_H_

#include <stdint.h>

#include <drivers/flash/flash.h>

#define FLASH_CACHE_IN_RAM     1

#define FLASH_CACHE_USE_RAM_SECTION \
				OPTION_MODULE_GET(embox__driver__flash__flash_cache_ram, \
								BOOLEAN, use_ram_section)

#if FLASH_CACHE_USE_RAM_SECTION
#define FLASH_CACHE_SECTION      __attribute__((section(".dfs_cache_section")))
#else
#define FLASH_CACHE_SECTION
#endif

#define FLASH_CACHE_DEF(cache_name, word_size, block_size) \
			static uint8_t cache_name ## _block_buffer[word_size] \
						FLASH_CACHE_SECTION	\
						__attribute__ ((aligned(block_size)))

#define FLASH_CACHE_GET(fdev, cache_name) \
				((uintptr_t) cache_name ## _block_buffer)

extern int flash_cache_clean(struct flash_dev *flashdev, uint32_t block);

extern int flash_cache_load(struct flash_dev *flashdev,
							 uint32_t to, uint32_t from, int len);

extern int flash_cache_write(struct flash_dev *flashdev,
						uint32_t offset, const void *buff, size_t len);

extern int flash_cache_restore(struct flash_dev *flashdev,
											 uint32_t to, uint32_t from);

#define CACHE_OFFSET(fdev)           \
			((uintptr_t)fdev->fld_cache)

#endif /* _FLASH_CACHE_RAM_H_ */
