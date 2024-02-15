/**
 * @file
 * @brief VFS-independent functions related to block devices
 *
 * @date   14 Apr 2015
 * @author Denis Deryugin
 */

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <stddef.h>
#include <string.h>

#include <drivers/block_dev.h>
#include <framework/mod/options.h>
#include <fs/bcache.h>
#include <lib/libds/array.h>
#include <lib/libds/indexator.h>
#include <mem/misc/pool.h>
#include <mem/page.h>
#include <mem/phymem.h>
#include <util/math.h>

extern struct idesc_ops idesc_bdev_ops;

#define DEFAULT_BDEV_BLOCK_SIZE OPTION_GET(NUMBER, default_block_size)

ARRAY_SPREAD_DEF(const struct block_dev_module, __block_dev_registry);
POOL_DEF(cache_pool, struct block_dev_cache, MAX_BDEV_QUANTITY);
POOL_DEF(blockdev_pool, struct block_dev, MAX_BDEV_QUANTITY);
INDEX_DEF(block_dev_idx, 0, MAX_BDEV_QUANTITY);

static struct block_dev *devtab[MAX_BDEV_QUANTITY];

struct block_dev **get_bdev_tab(void) {
	return &devtab[0];
}

static int block_dev_cache_free(void *dev) {
	struct block_dev *bdev;
	struct block_dev_cache *cache;

	if (NULL == dev) {
		return -1;
	}
	bdev = block_dev(dev);

	cache = bdev->cache;
	if (NULL == cache) {
		return 0;
	}

	phymem_free(cache->pool, cache->depth * cache->blkfactor);
	pool_free(&cache_pool, cache);

	return 0;
}

void block_dev_free(struct block_dev *dev) {
	int idx;

	assert(dev);

	idx = block_dev_id(dev);

	devtab[idx] = NULL;
	index_free(&block_dev_idx, idx);
	pool_free(&blockdev_pool, dev);
}

int block_devs_init(void) {
	int ret;
	const struct block_dev_module *bdev_module;

	array_spread_foreach_ptr(bdev_module, __block_dev_registry) {
		if (bdev_module->dev_drv->bdo_probe != NULL) {
			ret = bdev_module->dev_drv->bdo_probe(NULL, NULL);
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}

struct block_dev_module *block_dev_lookup(const char *bd_name) {
	struct block_dev_module *bdev_module;

	array_spread_foreach_ptr(bdev_module, __block_dev_registry) {
		if (0 == strcmp(bdev_module->name, bd_name)) {
			return bdev_module;
		}
	}

	return NULL;
}

struct block_dev *block_dev_find(const char *bd_name) {
	int i;

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (devtab[i] && 0 == strcmp(block_dev_name(devtab[i]), bd_name)) {
			return devtab[i];
		}
	}

	return NULL;
}

int block_dev_max_id(void) {
	return MAX_BDEV_QUANTITY;
}

struct block_dev *block_dev_by_id(int id) {
	if (id < 0 || id >= MAX_BDEV_QUANTITY) {
		return NULL;
	}

	return devtab[id];
}

struct block_dev *block_dev(void *dev) {
	return (struct block_dev *)dev;
}

int block_dev_read_buffered(struct block_dev *bdev, char *buffer, size_t count,
    size_t offset) {
	size_t blksize;
	int blkno, cplen, cursor, res, i;
	struct buffer_head *bh;

	assert(bdev);
	assert(bdev->driver);

	if (NULL == bdev->driver->bdo_read) {
		return -ENOSYS;
	}
	if (offset + count > bdev->size) {
		return -EIO;
	}
	blksize = block_dev_block_size(bdev);
	blkno = offset / blksize;
	cplen = min(count, blksize - offset % blksize);

	for (cursor = 0, i = 0; count != 0;
	     i++, count -= cplen, cursor += cplen, cplen = min(count, blksize)) {
		bh = bcache_getblk_locked(bdev, blkno + i, blksize);
		{
			if (buffer_new(bh)) {
				if ((blksize
				        != (res = bdev->driver->bdo_read(bdev, bh->data,
				                blksize, blkno + i)))
				    || (0 != (res = buffer_decrypt(bh)))) {
					bcache_buffer_unlock(bh);
					return res;
				}
				buffer_clear_flag(bh, BH_NEW);
			}
			memcpy(buffer + cursor, bh->data + (i == 0 ? offset % blksize : 0),
			    cplen);
		}
		bcache_buffer_unlock(bh);
	}

	return cursor;
}

int block_dev_write_buffered(struct block_dev *bdev, const char *buffer,
    size_t count, size_t offset) {
	size_t blksize;
	int blkno, cplen, cursor, res, i;
	struct buffer_head *bh;

	assert(bdev);

	if (NULL == bdev->driver->bdo_write) {
		return -ENOSYS;
	}
	if (offset + count > bdev->size) {
		return -EIO;
	}

	blksize = bdev->block_size;
	blkno = offset / blksize;
	cplen = min(count, blksize - offset % blksize);

	for (cursor = 0, i = 0; count != 0;
	     i++, count -= cplen, cursor += cplen, cplen = min(count, blksize)) {
		bh = bcache_getblk_locked(bdev, blkno + i, blksize);
		{
			if (buffer_new(bh)) {
				if (cplen < blksize) {
					if (blksize
					        != (res = bdev->driver->bdo_read(bdev, bh->data,
					                blksize, blkno + i))
					    || 0 != (res = buffer_decrypt(bh))) {
						bcache_buffer_unlock(bh);
						return res;
					}
				}
				buffer_clear_flag(bh, BH_NEW);
			}
			memcpy(bh->data + (i == 0 ? offset % blksize : 0), buffer + cursor,
			    cplen);
			/**
			 * Blocks are stored in the buffer cache in a decrypted state.
			 * Therefore first we encrypt block, then write it onto disk and then decrypt block.
			 */
			buffer_encrypt(bh);
			if (blksize
			    != (res = bdev->driver->bdo_write(bdev, bh->data, blksize,
			            blkno + i))) {
				buffer_decrypt(bh);
				bcache_buffer_unlock(bh);
				return res;
			}
			buffer_decrypt(bh);
		}
		bcache_buffer_unlock(bh);
	}

	return cursor;
}

int block_dev_read(void *dev, char *buffer, size_t count, blkno_t blkno) {
	struct block_dev *bdev;
	size_t blksize;

	if (NULL == dev) {
		return -ENODEV;
	}
	bdev = block_dev(dev);

	if (blkno >= bdev->size / bdev->block_size) {
		return -EINVAL;
	}

	if (bdev->parent_bdev != NULL) {
		blkno += bdev->start_offset;
		bdev = bdev->parent_bdev;
	}

	blksize = block_dev_block_size(bdev);

	return block_dev_read_buffered(bdev, buffer, count, blkno * blksize);
}

int block_dev_write(void *dev, const char *buffer, size_t count,
    blkno_t blkno) {
	struct block_dev *bdev;
	size_t blksize;

	if (NULL == dev) {
		return -ENODEV;
	}

	bdev = block_dev(dev);

	if (blkno >= bdev->size / bdev->block_size) {
		return -EINVAL;
	}

	if (bdev->parent_bdev != NULL) {
		blkno += bdev->start_offset;
		bdev = bdev->parent_bdev;
	}

	blksize = block_dev_block_size(bdev);

	return block_dev_write_buffered(bdev, buffer, count, blkno * blksize);
}

int block_dev_ioctl(void *dev, int cmd, void *args, size_t size) {
	struct block_dev *bdev;

	if (NULL == dev) {
		return -ENODEV;
	}

	bdev = dev;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return bdev->size;
	case IOCTL_GETBLKSIZE:
		return bdev->block_size;
	default:
		assert(bdev->driver);
		if (NULL == bdev->driver->bdo_ioctl)
			return -ENOSYS;

		return bdev->driver->bdo_ioctl(bdev, cmd, args, 0);
	}
}

struct block_dev_cache *block_dev_cache_init(void *dev, int blocks) {
	int pagecnt;
	struct block_dev_cache *cache;
	struct block_dev *bdev;

	if (NULL == dev) {
		return NULL;
	}
	/* if reinit */
	block_dev_cache_free(dev);

	bdev = block_dev(dev);
	if (NULL == (cache = bdev->cache = pool_alloc(&cache_pool))) {
		return NULL;
	}

	cache->lastblkno = -1;
	cache->buff_cntr = -1;

	if (0 >= (cache->blksize = block_dev_block_size(bdev))) {
		return NULL;
	}

	/* cache size is a multiple of the memory page */
	pagecnt = 1;
	if (cache->blksize > PAGE_SIZE()) {
		pagecnt = cache->blksize / PAGE_SIZE();
		if (cache->blksize % PAGE_SIZE()) {
			pagecnt++;
		}
	}
	cache->blkfactor = pagecnt;

	if (NULL == (cache->pool = phymem_alloc(blocks * pagecnt))) {
		return NULL;
	}
	cache->depth = blocks;

	return cache;
}

struct block_dev_cache *block_dev_cached_read(void *dev, blkno_t blkno) {
	struct block_dev_cache *cache;
	struct block_dev *bdev;

	if (NULL == dev) {
		return NULL;
	}
	bdev = block_dev(dev);

	if (NULL == (cache = bdev->cache)) {
		return NULL;
	}

	/* set pointer to the buffer in pool */
	if (cache->lastblkno != blkno) {
		cache->buff_cntr++;
		cache->buff_cntr %= cache->depth;

		cache->data = cache->pool
		              + cache->buff_cntr * PAGE_SIZE() * cache->blkfactor;
		cache->blkno = blkno;

		block_dev_read(bdev, cache->data, cache->blksize, cache->blkno);
		cache->lastblkno = blkno;
	}

	return cache;
}

struct dev_module *block_dev_to_device(struct block_dev *dev) {
	assert(dev);

	return &dev->dev_module;
}

uint64_t block_dev_size(struct block_dev *dev) {
	assert(dev);

	return dev->size;
}

size_t block_dev_block_size(struct block_dev *dev) {
	assert(dev);

	return dev->block_size;
}

struct block_dev *block_dev_parent(struct block_dev *dev) {
	assert(dev);

	return dev->parent_bdev;
}

const char *block_dev_name(struct block_dev *dev) {
	assert(dev);

	return dev->dev_module.name;
}

dev_t block_dev_id(struct block_dev *dev) {
	assert(dev);

	return dev->dev_module.dev_id & DEVID_ID_MASK;
}

void *block_dev_priv(struct block_dev *dev) {
	return dev->dev_module.dev_priv;
}

struct block_dev *block_dev_create(const char *path,
    const struct block_dev_ops *driver, void *privdata) {
	struct block_dev *bdev;
	struct dev_module *devmod;
	size_t bdev_id;

	bdev = (struct block_dev *)pool_alloc(&blockdev_pool);
	if (NULL == bdev) {
		return NULL;
	}

	memset(bdev, 0, sizeof(struct block_dev));

	bdev_id = index_alloc(&block_dev_idx, INDEX_MIN);
	if (bdev_id == INDEX_NONE) {
		block_dev_free(bdev);
		return NULL;
	}
	devtab[bdev_id] = bdev;

	*bdev = (struct block_dev){
	    .driver = driver,
	    .block_size = DEFAULT_BDEV_BLOCK_SIZE,
	};

	devmod = dev_module_init(&bdev->dev_module, basename((char *)path),
	    &idesc_bdev_ops, privdata);
	devmod->dev_id = DEVID_BDEV | bdev_id;

	return bdev;
}

int block_dev_destroy(struct block_dev *dev) {
	for (int i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (devtab[i] && devtab[i]->parent_bdev == dev) {
			block_dev_destroy(devtab[i]);
		}
	}

	dev_module_deinit(&dev->dev_module);

	block_dev_free(dev);

	return 0;
}
