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
#include <string.h>

#include <util/math.h>

#include <drivers/flash/flash.h>

#define flash_cache_clean(flashdev, block)        \
						flash_erase(flashdev, block)

#define flash_cache_load(flashdev, to, from, len)      \
						flash_copy_aligned(flashdev, to,from,len)

#define flash_cache_write(flashdev, off,buf, len) \
						flash_write_aligned(flashdev, off, buf, len)

#define flash_cache_restore(flashdev, to, from)   \
						flash_copy_block(flashdev, to,from)

#define CACHE_OFFSET(fdev)                 \
			(fdev->fld_cache * fdev->block_info[0].block_size)

#endif /* _FLASH_CACHE_BLOCK_H_ */
