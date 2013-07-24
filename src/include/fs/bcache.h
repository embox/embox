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
#include <embox/block_dev.h>

#define BH_NEW    0x00000001
#define BH_LOCKED 0x00000002
#define BH_DIRTY  0x00000004

#define buffer_new(bh) (bh->flags | BH_NEW)
#define buffer_locked(bh) (bh->flags | BH_LOCKED)
#define buffer_dirty(bh) (bh->flags | BH_DIRTY)

struct buffer_head {
	block_dev_t *bdev;
	int block;
	size_t blocksize;
	int flags;
	char *data;
};

extern struct buffer_head *bcache_getblk(block_dev_t *bdev, int block, size_t size);
extern int bcache_flush_blk(struct buffer_head *bh);

#endif /* FS_BCACHE_H_ */
