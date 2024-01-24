/**
 * @file
 * @brief Buffer head - representation of the block (or group of blocks) on a disk
 *
 * @author  Alexander Kalmuk
 * @date    22.07.2013
 */

#ifndef FS_BUFFER_HEAD_H_
#define FS_BUFFER_HEAD_H_

#include <lib/libds/dlist.h>
#include <kernel/thread/sync/mutex.h>
#include <drivers/block_dev.h>
#include <fs/journal.h>

#define BH_NEW     0x00000001
#define BH_DIRTY   0x00000002
#define BH_JOURNAL 0x00000004

#define buffer_new(bh) (bh->flags & BH_NEW)
#define buffer_locked(bh) (bh->lock_count)
#define buffer_dirty(bh) (bh->flags & BH_DIRTY)
#define buffer_journal(bh) (bh->flags & BH_JOURNAL)

#define buffer_set_flag(bh, flag) bh->flags |= flag
#define buffer_clear_flag(bh, flag) bh->flags &= ~flag

/**
 * A buffer_head is used to map a single block within a disk
 */
struct buffer_head {
	struct block_dev *bdev;              /* device buffer_header is from */
	int block;                      /* start block number */
	size_t blocksize;               /* size of mapping */
	int flags;                      /* buffer state bitmap */
	struct mutex mutex;             /* synchronizes concurrent access to block */
	struct dlist_head bh_next;      /* link to global list of buffer_heads */
	char *data;                     /* pointer to block's data */
	int lock_count;			/* lock count to support multiplie locks */
	/*
	 * XXX Seems it is not better solution to have back reference to journal.
	 */
	journal_block_t *journal_block; /* pointer to corresponding up-to-date block from journal (if any) */
};

extern void buffer_encrypt(struct buffer_head *bh);
extern int buffer_decrypt(struct buffer_head *bh);

#endif /* FS_BUFFER_HEAD_H_ */
