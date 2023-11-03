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
#include <string.h>

#include <util/math.h>

#include <drivers/flash/flash.h>

#define FLASH_CACHE_IN_RAM     1

#define FLASH_CACHE_USE_RAM_SECTION \
				OPTION_MODULE_GET(embox__driver__flash__flash_cache_ram, \
								BOOLEAN, use_ram_section)

static inline int flash_cache_clean(struct flash_dev *flashdev, uint32_t block) {
	return 0;
}

static inline int flash_cache_load(struct flash_dev *flashdev,
							 uint32_t to, uint32_t from, int len) {
	char b[32];

	while (len > 0) {
		int tmp_len;

		tmp_len = min(len, sizeof(b));

		if (0 > flash_read_aligned(flashdev, from, b, tmp_len)) {
			return -1;
		}
		memcpy((void *)((uintptr_t)to), b, tmp_len);

		len -= tmp_len;
		to += tmp_len;
		from += tmp_len;
	}

	return 0;
}

static inline int flash_cache_write(struct flash_dev *flashdev,
						uint32_t offset, const void *buff, size_t len) {
	memcpy((void *)((uintptr_t)offset), buff, len);
	return 0;
}

static inline int flash_cache_restore(struct flash_dev *flashdev,
											 uint32_t to, uint32_t from) {
	int res;
	int flash_block_size;

	flash_block_size = flashdev->block_info[0].block_size;

	flash_erase(flashdev, to);
	res = flash_write_aligned(flashdev,
		to * flash_block_size, (void *)((uintptr_t)from), flash_block_size);

	return res;
}

#define CACHE_OFFSET(fdev)           \
			((uintptr_t)fdev->fld_cache)

#endif /* _FLASH_CACHE_RAM_H_ */
