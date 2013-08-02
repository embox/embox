/**
 * @file
 * @brief Buffer cache
 *
 * @author  Alexander Kalmuk
 * @date    22.07.2013
 */

#ifndef FS_BCACHE_H_
#define FS_BCACHE_H_

#include <util/dlist.h>
#include <kernel/thread/sync/mutex.h>
#include <embox/block_dev.h>

#define BH_NEW    0x00000001
#define BH_LOCKED 0x00000002
#define BH_DIRTY  0x00000004

#define buffer_new(bh) (bh->flags & BH_NEW)
#define buffer_locked(bh) (bh->flags & BH_LOCKED)
#define buffer_dirty(bh) (bh->flags & BH_DIRTY)

#define buffer_set_flag(bh, flag) bh->flags |= flag
#define buffer_clear_flag(bh, flag) bh->flags &= ~flag

struct buffer_head {
	block_dev_t *bdev;
	int block;
	size_t blocksize;
	int flags;
	struct mutex mutex;
	char *data;
};

static inline void bcache_buffer_lock(struct buffer_head *bh) {
	mutex_lock(&bh->mutex);
	buffer_set_flag(bh, BH_LOCKED);
}

static inline void bcache_buffer_unlock(struct buffer_head *bh) {
	buffer_clear_flag(bh, BH_LOCKED);
	mutex_unlock(&bh->mutex);
}

/**
 * @return
 *   Block with number @a block and with size @a size from device @a bdev.
 *   Block is returned in locked state.
 */
extern struct buffer_head *bcache_getblk_locked(block_dev_t *bdev, int block, size_t size);

/**
 * Force flushing of block @a bh on disk.
 *
 * @return
 *   Result of writing to disk.
 */
extern int bcache_flush_blk(struct buffer_head *bh);

/**
 * XXX it is temporary function replaces function that flushes all blocks of specified file
 *  (used by fsync)
 */
extern int bcache_flush_all(void);

#endif /* FS_BCACHE_H_ */
