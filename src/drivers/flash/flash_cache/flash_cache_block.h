/**
 * @file
 *
 * @author Denis Deryugin
 * @date 26 Dec 2014
 * 
 */

#ifndef _FLASH_CACHE_BLOCK_H_
#define _FLASH_CACHE_BLOCK_H_

#include <stdint.h>

#include <drivers/flash/flash.h>

#define FLASH_CACHE_DEF(cache_name, word_size, block_size)

#define FLASH_CACHE_GET(fdev, cache_name) \
			((uintptr_t)fdev->block_info[0].blocks - 1)

extern int flash_cache_clean(struct flash_dev *flashdev);

#define flash_cache_load(flashdev, to, from, len)      \
						flash_copy_aligned(flashdev, to,from,len)

#define flash_cache_write(flashdev, off,buf, len) \
						flash_write_aligned(flashdev, off, buf, len)

#define flash_cache_restore(flashdev, to, from)   \
						flash_copy_block(flashdev, to,from)

#define CACHE_OFFSET(fdev)                 \
			(fdev->fld_cache * fdev->block_info[0].block_size)

#endif /* _FLASH_CACHE_BLOCK_H_ */
