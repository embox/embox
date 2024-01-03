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

/* @brief Write non-aligned raw data to non-erased NAND flash
 *
 * @param pos  Start position on disk
 * @param buff Source of the data
 * @param size Length of the data in bytes
 *
 * @returns Bytes written or negative error code
 */
int flash_write_buffered(struct flash_dev *flashdev,
					int pos, void *buff, size_t size) {
	int blk;
	int err;
	int block_size;

	assert(buff);

	blk = flash_get_block_by_offset(flashdev, pos);

	err = 0;
	
	while(0 < size) {
		int off_in_blk;

		block_size = flash_get_block_size(flashdev, blk);

		flash_cache_clean(flashdev);

		off_in_blk = (pos % block_size);

		if ( 0 != off_in_blk) {
			/* load into the cache unchanged bytes from the block */
			err = flash_cache_load(flashdev,
						CACHE_OFFSET(flashdev) + 0,
						flash_get_offset_by_block(flashdev, blk),
						off_in_blk);
			if (err != 0) {
				break;
			}
		}

		/* store data in cache */
		if (size <= (block_size - off_in_blk)) {
			/*  if data fits in the block */
			err = flash_cache_write(flashdev,
							CACHE_OFFSET(flashdev) + off_in_blk, buff,
							size);
			if (err != 0) {
				break;
			}

			pos += size;
			size = 0;
		} else {
			/*  if data doesn't fit in the block */
			err = flash_cache_write(flashdev,
							CACHE_OFFSET(flashdev) + off_in_blk, buff,
							block_size - off_in_blk);
			if (err != 0) {
				break;
			}

			pos += block_size - off_in_blk;
			size -= block_size - off_in_blk;
		}

		off_in_blk = (pos % block_size);

		if (0 != off_in_blk) {
			/* load into the cache unchanged bytes from a tail of the block */
			flash_cache_load(flashdev,
					CACHE_OFFSET(flashdev) + off_in_blk,
					pos,
					block_size - off_in_blk);
			if (err != 0) {
				break;
			}
		}

		/* store whole block from cache into the flash */
		flash_cache_restore(flashdev, blk, flashdev->fld_cache);
		if (err != 0) {
			break;
		}
		blk ++;
	}

	return err;
}
