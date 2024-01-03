/**
 * @file
 * @brief Buffer cache
 *
 * @author  Alexander Kalmuk
 * @date    22.07.2013
 */

#ifndef FS_BCACHE_H_
#define FS_BCACHE_H_

#include <fs/buffer_head.h>

static inline void bcache_buffer_lock(struct buffer_head *bh) {
	mutex_lock(&bh->mutex);
	bh->lock_count++;
}

static inline void bcache_buffer_unlock(struct buffer_head *bh) {
	bh->lock_count--;
	mutex_unlock(&bh->mutex);
}

/**
 * @return
 *   Block with number @a block and with size @a size from device @a bdev.
 *   Block is returned in locked state.
 */
extern struct buffer_head *bcache_getblk_locked(struct block_dev *bdev, int block, size_t size);

#endif /* FS_BCACHE_H_ */
