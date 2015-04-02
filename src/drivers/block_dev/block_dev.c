/**
 * @file
 * @brief
 *
 * @date 31.07.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include <mem/phymem.h>
#include <mem/misc/pool.h>
#include <util/array.h>
#include <util/indexator.h>
#include <util/math.h>

#include <embox/unit.h>

#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/file_desc.h>
#include <fs/bcache.h>
#include <fs/file_operation.h>

#include <embox/block_dev.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)

EMBOX_UNIT_INIT(blockdev_init);

POOL_DEF(blockdev_pool, struct block_dev, MAX_DEV_QUANTITY);
POOL_DEF(cache_pool, struct block_dev_cache, MAX_DEV_QUANTITY);
INDEX_DEF(block_dev_idx,0,MAX_DEV_QUANTITY);

static struct block_dev *devtab[MAX_DEV_QUANTITY];

static int block_dev_cache_free(void *dev_id);

static int bdev_open(struct node *node, struct file_desc *file_desc, int flags) {
	return 0;
}

static int bdev_close(struct file_desc *desc) {
	return 0;
}

static size_t bdev_read(struct file_desc *desc, void *buf, size_t size) {
	int n_read = block_dev_read_buffered((block_dev_t *) desc->node->nas->fi->privdata,
			buf, size, desc->cursor);
	if (n_read > 0) {
		desc->cursor += n_read;
	}
	return n_read;
}

static size_t bdev_write(struct file_desc *desc, void *buf, size_t size) {
	int n_write = block_dev_write_buffered((block_dev_t *) desc->node->nas->fi->privdata,
			buf, size, desc->cursor);
	if (n_write > 0) {
		desc->cursor += n_write;
	}

	return n_write;
}

static int bdev_ioctl(struct file_desc *desc, int request, ...) {
	return -ENOTSUP;
}

static struct kfile_operations blockdev_fop = {
	.open = bdev_open,
	.close = bdev_close,
	.read = bdev_read,
	.write = bdev_write,
	.ioctl = bdev_ioctl,

};

static struct filesystem *blockdev_fs;

static int blockdev_init(void) {
	blockdev_fs = filesystem_create("empty");
	blockdev_fs->file_op = &blockdev_fop;

	return 0;
}

struct block_dev *block_dev(void *dev) {
	return (struct block_dev *)dev;
}

struct block_dev *block_dev_create(char *path, void *driver, void *privdata) {
	block_dev_t *bdev;
	size_t bdev_id;
	struct path node, root;
	struct nas *nas;
	struct node_fi *node_fi;

	bdev = (block_dev_t *) pool_alloc(&blockdev_pool);
	if (NULL == bdev) {
		return NULL;
	}

	memset(bdev, 0, sizeof(block_dev_t));

	bdev_id = index_alloc(&block_dev_idx, INDEX_MIN);
	if (bdev_id == INDEX_NONE) {
		pool_free(&blockdev_pool, bdev);
		return NULL;
	}
	bdev->id = (dev_t)bdev_id;

	devtab[bdev->id] = bdev;

	bdev->driver = driver;
	bdev->privdata = privdata;

	vfs_get_root_path(&root);

	if (0 != vfs_create(&root, path, S_IFBLK | S_IRALL | S_IWALL, &node)) {
		index_free(&block_dev_idx, bdev->id);
		pool_free(&blockdev_pool, bdev);
		return NULL;
	}

	strncpy (bdev->name, node.node->name, NAME_MAX);
	bdev->dev_node = node.node;

	nas = node.node->nas;
	nas->fs = blockdev_fs;
	node_fi = nas->fi;
	node_fi->privdata = bdev;
	node_fi->ni.size = 0;/*TODO*/
	node_fi->ni.mtime = 0;/*TODO*/

	return bdev;
}

int block_dev_read_buffered(block_dev_t *bdev, char *buffer, size_t count, size_t offset) {
	int blksize, blkno, cplen, cursor;
	int res, i;
	struct buffer_head *bh;

	assert(bdev);

	if (NULL == bdev->driver->read) {
		return -ENOSYS;
	}
	if (offset + count > bdev->size) {
		return -EIO;
	}
	blksize = block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);
	if (blksize < 0) {
		return blksize;
	}
	blkno = offset / blksize;
	cplen = min(count, blksize - offset % blksize);

	for (cursor = 0, i = 0; count != 0;
			i++, count -= cplen, cursor += cplen, cplen = min(count, blksize)) {
		bh = bcache_getblk_locked(bdev, blkno + i, blksize);
		{
			if (buffer_new(bh)) {
				if (blksize != (res = bdev->driver->read(bdev, bh->data, blksize, blkno + i))
						|| 0 != (res = buffer_decrypt(bh))) {
					bcache_buffer_unlock(bh);
					return res;
				}
				buffer_clear_flag(bh, BH_NEW);
			}
			memcpy(buffer + cursor, bh->data + (i == 0 ? offset % blksize : 0), cplen);
		}
		bcache_buffer_unlock(bh);
	}

	return cursor;
}

int block_dev_write_buffered(block_dev_t *bdev, const char *buffer, size_t count, size_t offset) {
	int blksize, blkno, cplen, cursor;
	int res, i;
	struct buffer_head *bh;

	assert(bdev);

	if (NULL == bdev->driver->write) {
		return -ENOSYS;
	}
	if (offset + count > bdev->size) {
		return -EIO;
	}

	blksize = block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);
	if (blksize < 0) {
		return blksize;
	}

	blkno = offset / blksize;
	cplen = min(count, blksize - offset % blksize);

	for (cursor = 0, i = 0; count != 0;
			i++, count -= cplen, cursor += cplen, cplen = min(count, blksize)) {
		bh = bcache_getblk_locked(bdev, blkno + i, blksize);
		{
			if (buffer_new(bh)) {
				if (cplen < blksize) {
					if (blksize != (res = bdev->driver->read(bdev, bh->data, blksize, blkno + i))
							|| 0 != (res = buffer_decrypt(bh))) {
						bcache_buffer_unlock(bh);
						return res;
					}
				}
				buffer_clear_flag(bh, BH_NEW);
			}
			memcpy(bh->data + (i == 0 ? offset % blksize : 0), buffer + cursor, cplen);
			/**
			 * Blocks are stored in the buffer cache in a decrypted state.
			 * Therefore first we encrypt block, then write it onto disk and then decrypt block.
			 */
			buffer_encrypt(bh);
			if (blksize != (res = bdev->driver->write(bdev, bh->data,
					blksize, blkno + i))) {
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
	block_dev_t *bdev;
	int blksize;

	if (NULL == dev) {
		return -ENODEV;
	}
	bdev = block_dev(dev);

	blksize = block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);
	if (blksize < 0) {
		return blksize;
	}
	return block_dev_read_buffered(bdev, buffer, count, blkno * blksize);
}

int block_dev_write(void *dev, const char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *bdev;
	int blksize;

	if (NULL == dev) {
		return -ENODEV;
	}

	bdev = block_dev(dev);

	blksize = block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);
	if (blksize < 0) {
		return blksize;
	}
	return block_dev_write_buffered(bdev, buffer, count, blkno * blksize);
}

int block_dev_ioctl(void *dev, int cmd, void *args, size_t size) {
	block_dev_t *bdev;

	if (NULL == dev) {
		return -ENODEV;
	}
	bdev = block_dev(dev);

	if (NULL == bdev->driver->ioctl) {
		return -ENOSYS;
	}

	return bdev->driver->ioctl(bdev, cmd, args, size);
}


block_dev_cache_t *block_dev_cache_init(void *dev, int blocks) {
	int pagecnt;
	block_dev_cache_t *cache;
	block_dev_t *bdev;

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

	if (0 >= (cache->blksize =
			block_dev_ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0))) {
		return NULL;
	}

	/* cache size is a multiple of the memory page */
	pagecnt = 1;
	if(cache->blksize > PAGE_SIZE()) {
		pagecnt = cache->blksize / PAGE_SIZE();
		if(cache->blksize % PAGE_SIZE()) {
			pagecnt++;
		}
	}
	cache->blkfactor = pagecnt;

	if (NULL == (cache->pool = phymem_alloc(blocks * pagecnt))) {
		return NULL;
	}
	cache->depth = blocks;

	return  cache;
}

block_dev_cache_t *block_dev_cached_read(void *dev, blkno_t blkno) {
	block_dev_cache_t *cache;
	block_dev_t *bdev;

	if (NULL == dev) {
		return NULL;
	}
	bdev = block_dev(dev);

	if(NULL == (cache = bdev->cache)) {
		return NULL;
	}

	/* set pointer to the buffer in pool */
	if(cache->lastblkno != blkno) {
		cache->buff_cntr++;
		cache->buff_cntr %= cache->depth;

		cache->data = cache->pool + cache->buff_cntr * PAGE_SIZE() * cache->blkfactor;
		cache->blkno = blkno;

		block_dev_read(bdev, cache->data, cache->blksize, cache->blkno);
		cache->lastblkno = blkno;
	}

	return cache;
}

static int block_dev_cache_free(void *dev) {
	block_dev_t *bdev;
	block_dev_cache_t *cache;

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

	return  0;
}

int block_dev_destroy (void *dev) {
	block_dev_t *bdev;

	bdev = block_dev(dev);

	vfs_del_leaf(bdev->dev_node);

	block_dev_cache_free(bdev);

	devtab[bdev->id] = NULL;
	index_free(&block_dev_idx, bdev->id);

	pool_free(&blockdev_pool, bdev);

	return 0;
}


ARRAY_SPREAD_DEF(const block_dev_module_t, __block_dev_registry);


int block_devs_init(void) {
	int ret;
	const block_dev_module_t *bdev_module;

	array_spread_foreach_ptr(bdev_module, __block_dev_registry) {
		if (bdev_module->init != NULL) {
			ret = bdev_module->init(NULL);
			if (ret != 0) {
				return ret;
			}
		}

	}

	return 0;
}

block_dev_module_t *block_dev_lookup(const char *bd_name) {
	block_dev_module_t *bdev_module;

	array_spread_foreach_ptr(bdev_module, __block_dev_registry) {
		if (0 == strcmp(bdev_module->name, bd_name)) {
			return bdev_module;
		}
	}

	return NULL;
}

