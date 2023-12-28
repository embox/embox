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
	int start_bk;
	int err;
	int block_size;

	assert(buff);

	block_size = flashdev->block_info[0].block_size;

	start_bk = pos / block_size;

	err = 0;
	
	while(size) {
		block_size = flashdev->block_info[0].block_size;

		flash_cache_clean(flashdev, flashdev->fld_cache);

		if ( 0 != (pos % block_size)) {
			/* load into the cache unchanged bytes from the block */
			err = flash_cache_load(flashdev,
						CACHE_OFFSET(flashdev),
						start_bk * block_size,
						(pos % block_size));
			if (err != 0) {
				break;
			}
		}

		/* store data in cache */
		if (size <= (block_size - (pos % block_size))) {
			/*  if data fits in the block */
			err = flash_cache_write(flashdev,
							CACHE_OFFSET(flashdev) + pos, buff,
							size);
			if (err != 0) {
				break;
			}

			pos += size;
			size = 0;
		} else {
			/*  if data doesn't fit in the block */
			err = flash_cache_write(flashdev,
							CACHE_OFFSET(flashdev) + pos, buff, 
							block_size - (pos % block_size) );
			if (err != 0) {
				break;
			}

			pos += block_size - (pos % block_size);
			size -= block_size - (pos % block_size);
		}

		if (0 != (pos % block_size)) {
			/* load into the cache unchanged bytes from a tail of the block */
			flash_cache_load(flashdev,
					CACHE_OFFSET(flashdev) + pos,
					start_bk * block_size + pos,
					block_size - (pos % block_size));
			if (err != 0) {
				break;
			}
		}

		/* store whole block from cache into the flash */
		flash_cache_restore(flashdev, start_bk, flashdev->fld_cache);		
		if (err != 0) {
			break;
		}
		start_bk ++;
	}

	return err;
}
