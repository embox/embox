/**
 * @file
 * @brief Buffer cache
 *
 * @author  Alexander Kalmuk
 * @date    22.07.2013
 */

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <lib/libds/hashtable.h>

#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>

#include <fs/bcache.h>

#include <embox/unit.h>
EMBOX_UNIT_INIT(bcache_init);

#define BCACHE_SIZE   OPTION_GET(NUMBER, bcache_size)
#define BCACHE_ALIGN  OPTION_GET(NUMBER, bcache_align)

POOL_DEF(buffer_head_pool, struct buffer_head, BCACHE_SIZE);
static DLIST_DEFINE(bh_list);


static size_t bh_hash(void *key);
static int bh_cmp(void *key1, void *key2);
HASHTABLE_DEF(bcache_ht, BCACHE_SIZE / 10, bh_hash, bh_cmp);
POOL_DEF(bcach_ht_item_pool, struct hashtable_item, BCACHE_SIZE);

static struct hashtable *bcache = &bcache_ht;
static struct mutex bcache_mutex;
static int graw_buffers(struct block_dev *bdev, int block, size_t size);
static void free_more_memory(size_t size);

struct buffer_head *bcache_getblk_locked(struct block_dev *bdev, int block, size_t size) {
	struct buffer_head key = { .bdev = bdev, .block = block };
	struct buffer_head *bh;

	assert(bdev);

	mutex_lock(&bcache_mutex);
	while (1) {
		bh = (struct buffer_head *)hashtable_get(bcache, &key);

		if (bh) {
			assert(size == bh->blocksize);
			bcache_buffer_lock(bh);
			mutex_unlock(&bcache_mutex);
			return bh;
		}

		while (-1 == graw_buffers(bdev, block, size)) {
			free_more_memory(size);
		}
	}
	mutex_unlock(&bcache_mutex);

	assert(0); /* UNREACHABLE */
	return NULL;
}

static void free_more_memory(size_t size) {
	struct buffer_head *bh = NULL;
	struct hashtable_item *ht_item;

	/* Free everything that we can free */
	dlist_foreach_entry(bh, &bh_list, bh_next) {
		if (buffer_locked(bh)) {
			continue;
		}

		bcache_buffer_lock(bh);
		{
			if (buffer_journal(bh)) {
				bcache_buffer_unlock(bh);
				continue;
			}

			if (buffer_dirty(bh)) {
				assert(bh->bdev && bh->bdev->driver);
				assert(bh->bdev->driver->bdo_write);
				/* Write directly to disk */
				bh->bdev->driver->bdo_write(bh->bdev, bh->data, bh->blocksize, bh->block);
			}

			dlist_del(&bh->bh_next);
			ht_item = hashtable_del(bcache, bh);
		}
		bcache_buffer_unlock(bh);

		sysfree(bh->data);
		pool_free(&bcach_ht_item_pool, ht_item);
		pool_free(&buffer_head_pool, bh);
	}
}

static int graw_buffers(struct block_dev *bdev, int block, size_t size) {
	struct buffer_head *bh;
	struct hashtable_item *ht_item;

	bh = pool_alloc(&buffer_head_pool);

	if (!bh) {
		return -1;
	}

	memset(bh, 0, sizeof(struct buffer_head));

	buffer_set_flag(bh, BH_NEW);
	mutex_init(&bh->mutex);
	dlist_head_init(&bh->bh_next);
	bh->bdev = bdev;
	bh->block = block;
	bh->blocksize = size;
	bh->data = sysmemalign(BCACHE_ALIGN, size);

	if (!bh->data) {
		pool_free(&buffer_head_pool, bh);
		return -1;
	}
	ht_item = pool_alloc(&bcach_ht_item_pool);
	if (!ht_item) {
		sysfree(bh->data);
		pool_free(&buffer_head_pool, bh);
		return -1;
	}
	ht_item = hashtable_item_init(ht_item, bh, bh);
	hashtable_put(bcache, ht_item);

	dlist_add_next(&bh->bh_next, &bh_list);

	return 0;
}

static size_t bh_hash(void *key) {
	return ((struct buffer_head *)key)->block;
}

static int bh_cmp(void *key1, void *key2) {
	struct buffer_head *bh1 = (struct buffer_head *)key1;
	struct buffer_head *bh2 = (struct buffer_head *)key2;
	int cmp_bdev;

	cmp_bdev = bh1->bdev < bh2->bdev ? -1 : bh1->bdev > bh2->bdev;
	if (!cmp_bdev) {
		return (bh1->block < bh2->block ? -1 : bh1->block > bh2->block);
	}

	return cmp_bdev;
}

static int bcache_init(void) {

	mutex_init(&bcache_mutex);

	return 0;
}
