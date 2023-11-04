/**
 * @file
 *
 * @author Anton Bondarev
 * @date   19 Oct 2023
 */

#include <stdint.h>
#include <stddef.h>

#include <drivers/flash/flash.h>
#include <drivers/flash/flash_cache.h>


/* @brief Write non-aligned raw data to \b non-erased NAND flash
 * @param pos  Start position on disk
 * @param buff Source of the data
 * @param size Length of the data in bytes
 *
 * @returns Bytes written or negative error code
 */
int flash_write_buffered(struct flash_dev *flashdev,
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
