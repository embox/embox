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

#if USE_RAM_AS_CACHE

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

static int flash_cache_clean(struct flash_dev *flashdev, uint32_t block) {
	return 0;
}

static int flash_cache_load(struct flash_dev *flashdev,
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

#else /* !USE_RAM_AS_CACHE */

#define flash_cache_clean(flashdev, block)        \
						flash_erase(flashdev, block)

uintptr_t flash_cache_addr(struct flash_dev *flashdev) {
	return flashdev->block_info[0].blocks - 1;
}

#define flash_cache_load(flashdev, to, from, len)      \
						flash_copy_aligned(flashdev, to,from,len)

#define flash_cache_write(flashdev, off,buf, len) \
						flash_write_aligned(flashdev, off, buf, len)

#define flash_cache_restore(flashdev, to, from)   \
						flash_copy_block(flashdev, to,from)

#define CACHE_OFFSET(fdev)                 \
			(fdev->fld_cache * fdev->block_info[0].block_size)

#endif /* USE_RAM_AS_CACHE */

/* @brief Write non-aligned raw data to \b non-erased NAND flash
 * @param pos  Start position on disk
 * @param buff Source of the data
 * @param size Length of the data in bytes
 *
 * @returns Bytes written or negative error code
 */
int dfs_write_buffered(struct flash_dev *flashdev,
					int pos, void *buff, size_t size) {
	int start_bk;
	int last_bk;
	int bk;
	int err;
	int flash_block_size;
	uint32_t buff_bk = flashdev->fld_cache;

	assert(buff);

	flash_block_size = flashdev->block_info[0].block_size;

	start_bk = pos / flash_block_size;
	last_bk = (pos + size) / flash_block_size;

	pos %= flash_block_size;

	err = 0;

	flash_cache_clean(flashdev, buff_bk);

	flash_cache_load(flashdev,
				CACHE_OFFSET(flashdev), start_bk * flash_block_size, pos);

	if (start_bk == last_bk) {
		err = flash_cache_write(flashdev,
							CACHE_OFFSET(flashdev) + pos, buff, size);
		if (err) {
			return err;
		}
		pos += size;
	} else {
		flash_write_aligned(flashdev,
					CACHE_OFFSET(flashdev) + pos, buff, flash_block_size - pos);
		flash_copy_block(flashdev, start_bk, buff_bk);
		buff += flash_block_size - pos;
		pos = (pos + size) % flash_block_size;

		for (bk = start_bk + 1; bk < last_bk; bk++) {
			flash_erase(flashdev, bk);

			err = flash_write_aligned(flashdev,
							 bk * flash_block_size, buff, flash_block_size);
			if(err) {
				return err;
			}
			buff += flash_block_size;
		}

		flash_erase(flashdev, buff_bk);
		flash_write_aligned(flashdev, CACHE_OFFSET(flashdev), buff, pos);
	}

	flash_cache_load(flashdev,
						CACHE_OFFSET(flashdev) + pos,
						last_bk * flash_block_size + pos,
						flash_block_size - pos);
	flash_cache_restore(flashdev, last_bk, buff_bk);

	return 0;
}
