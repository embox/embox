/**
 * @file
 *
 * @author Anton Bondarev
 * @date   19 Oct 2023
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <drivers/flash/flash.h>
#include <drivers/flash/flash_cache.h>

int flash_cache_clean(struct flash_dev *flashdev, uint32_t block) {
	return 0;
}

int flash_cache_load(struct flash_dev *flashdev,
							 uint32_t to, uint32_t from, int len) {
	flash_read(flashdev, from, (void *)(uintptr_t)to, len);
#if 0
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
#endif
	return 0;
}

int flash_cache_write(struct flash_dev *flashdev,
						uint32_t offset, const void *buff, size_t len) {
	memcpy((void *)((uintptr_t)offset), buff, len);
	return 0;
}

int flash_cache_restore(struct flash_dev *flashdev,
								uint32_t to, uint32_t from) {
	int res;
	int flash_block_size;
	struct flash_block_info *to_fbi = flash_block_info_by_block(flashdev, to);

	flash_block_size = to_fbi->block_size;

	flash_erase(flashdev, to);

	res = flash_write(flashdev, to * flash_block_size, 
					(const void *)flashdev->fld_cache, flash_block_size);

	return res;
}
