/**
 * @file
 * @brief Buffer cache
 *
 * @author  Alexander Kalmuk
 * @date    22.07.2013
 */

#include <fs/bcache.h>
#include <embox/unit.h>
#include <mem/misc/pool.h>
#include <stdlib.h>
#include <util/hashtable.h>
#include <string.h>

EMBOX_UNIT_INIT(bcache_init);

#define BCACHE_SIZE OPTION_GET(NUMBER, bcache_size)

POOL_DEF(buffer_head_pool, struct buffer_head, BCACHE_SIZE);
static DLIST_DEFINE(bh_list);

static struct hashtable *buffer_cache;
static struct mutex bcache_mutex;
static int graw_buffers(block_dev_t *bdev, int block, size_t size);
static void free_more_memory(size_t size);

struct buffer_head *bcache_getblk_locked(block_dev_t *bdev, int block, size_t size) {
	struct buffer_head key = { .bdev = bdev, .block = block };
	struct buffer_head *bh;

	while (1) {
		mutex_lock(&bcache_mutex);

		bh = (struct buffer_head *)hashtable_get(buffer_cache, &key);

		if (bh) {
			bcache_buffer_lock(bh);
			mutex_unlock(&bcache_mutex);
			return bh;
		}

		mutex_unlock(&bcache_mutex);

		if (-1 == graw_buffers(bdev, block, size)) {
			free_more_memory(size);
		}
	}

	return NULL;
}

static void free_more_memory(size_t size) {
	struct buffer_head *bh, *bhnext;

	mutex_lock(&bcache_mutex);

	dlist_foreach_entry(bh, bhnext, &bh_list, bh_next) {
		if ((int)size < 0) {
			break;
		}

		bcache_buffer_lock(bh);
		{
			if (buffer_journal(bh)) {
				bcache_buffer_unlock(bh);
				continue;
			}

			if (buffer_dirty(bh)) {
				/* Write directly to disk */
				bh->bdev->driver->write(bh->bdev, bh->data, bh->blocksize, bh->block);
			}

			dlist_del(&bh->bh_next);
			hashtable_del(buffer_cache, bh);
		}
		bcache_buffer_unlock(bh);

		size -= bh->blocksize;

		free(bh->data);
		pool_free(&buffer_head_pool, bh);
	}

	mutex_unlock(&bcache_mutex);
}

static int graw_buffers(block_dev_t *bdev, int block, size_t size) {
	struct buffer_head *bh;

	mutex_lock(&bcache_mutex);

	bh = pool_alloc(&buffer_head_pool);

	if (!bh) {
		goto error;
	}

	memset(bh, 0, sizeof(struct buffer_head));

	buffer_set_flag(bh, BH_NEW);
	mutex_init(&bh->mutex);
	dlist_head_init(&bh->bh_next);
	bh->bdev = bdev;
	bh->block = block;
	bh->blocksize = size;
	bh->data = malloc(size); /* TODO kmalloc */

	if (!bh->data) {
		pool_free(&buffer_head_pool, bh);
		goto error;
	}

	if (0 > hashtable_put(buffer_cache, bh, bh)) {
		free(bh->data);
		pool_free(&buffer_head_pool, bh);
		goto error;
	}

	dlist_add_next(&bh->bh_next, &bh_list);

	mutex_unlock(&bcache_mutex);

	return 0;

error:
	mutex_unlock(&bcache_mutex);
	return -1;
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
	/* XXX calculate size of hashtable */
	buffer_cache = hashtable_create(BCACHE_SIZE / 10, bh_hash, bh_cmp);

	if (!buffer_cache) {
		return -1;
	}

	mutex_init(&bcache_mutex);

	return 0;
}
