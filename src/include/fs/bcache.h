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
#include <fs/journal.h>

#define BH_NEW     0x00000001
#define BH_LOCKED  0x00000002
#define BH_DIRTY   0x00000004
#define BH_JOURNAL 0x00000008

#define buffer_new(bh) (bh->flags & BH_NEW)
#define buffer_locked(bh) (bh->flags & BH_LOCKED)
#define buffer_dirty(bh) (bh->flags & BH_DIRTY)
#define buffer_journal(bh) (bh->flags & BH_JOURNAL)

#define buffer_set_flag(bh, flag) bh->flags |= flag
#define buffer_clear_flag(bh, flag) bh->flags &= ~flag

/**
 * A buffer_head is used to map a single block within a disk
 */
struct buffer_head {
	block_dev_t *bdev;              /* device buffer_header is from */
	int block;                      /* start block number */
	size_t blocksize;               /* size of mapping */
	int flags;                      /* buffer state bitmap */
	struct mutex mutex;             /* synchronizes concurrent access to block */
	struct dlist_head bh_next;      /* link to global list of buffer_heads */
	char *data;                     /* pointer to block's data */
	/*
	 * XXX Seems it is not better solution to have back reference to journal.
	 */
	journal_block_t *journal_block; /* pointer to corresponding up-to-date block from journal (if any) */
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

#endif /* FS_BCACHE_H_ */
