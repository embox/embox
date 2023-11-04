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

//#define NAND_PAGE_SIZE         OPTION_GET(NUMBER, page_size)
//#define NAND_BLOCK_SIZE        OPTION_GET(NUMBER, block_size)


int flash_cache_clean(struct flash_dev *flashdev, uint32_t block) {
	return 0;
}

int flash_cache_load(struct flash_dev *flashdev,
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

int flash_cache_write(struct flash_dev *flashdev,
						uint32_t offset, const void *buff, size_t len) {
	memcpy((void *)((uintptr_t)offset), buff, len);
	return 0;
}

int flash_cache_restore(struct flash_dev *flashdev,
								uint32_t to, uint32_t from) {
	int res;
	int flash_block_size;

	flash_block_size = flashdev->block_info[0].block_size;

	flash_erase(flashdev, to);
	res = flash_write_aligned(flashdev,
		to * flash_block_size, (void *)((uintptr_t)from), flash_block_size);

	return res;
}
