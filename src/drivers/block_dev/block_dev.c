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
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/file_desc.h>
#include <mem/phymem.h>
#include <mem/misc/pool.h>
#include <util/array.h>
#include <util/indexator.h>
#include <limits.h>

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
	block_dev_t *dev = (block_dev_t *) desc->node->nas->fi->privdata;
	int blksize = block_dev_ioctl(dev, IOCTL_GETBLKSIZE, NULL, 0);
	int blkno, blkcount, blkoff, res;
	int pages = size / PAGE_SIZE();
	char *bigbuf;
	size_t err;

	if (pages == 0) {
		pages++;
	}

	if (blksize <= 0) {
		return -ENOTSUP;
	}

	if (NULL == (bigbuf = page_alloc(__phymem_allocator, pages))) {
		return -ENOMEM;
	}

	blkno = desc->cursor / blksize;
	blkoff = desc->cursor % blksize;
	blkcount = size / blksize + (size % blksize ? 1 : 0);

	err = 0;

	if (blkcount * blksize != (res = block_dev_read(dev, bigbuf, blkcount * blksize, blkno))) {
		err = -EIO;
		goto out;
	}

	memcpy(buf, bigbuf + blkoff, size);
	err = size;

out:
	page_free(__phymem_allocator, bigbuf, pages);
	return err;
}

static size_t bdev_write(struct file_desc *desc, void *buf, size_t size) {
	return -ENOTSUP;
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
	blockdev_fs = filesystem_alloc("empty");
	blockdev_fs->file_op = &blockdev_fop;

	return 0;
}

/*
static block_dev_module_t *block_dev_find(char *name) {
	block_dev_module_t *b_dev;
	size_t i;

	for(i = 0; i < ARRAY_SPREAD_SIZE(__block_dev_registry); i++) {
		if(!strcmp(__block_dev_registry[i].name, name)) {
			b_dev = (block_dev_module_t *) &(__block_dev_registry[i]);
			return b_dev;
		}
	}

	return NULL;
}
*/

block_dev_t *block_dev(void *dev) {

	return (block_dev_t *)dev;
}

struct block_dev *block_dev_create(char *path, void *driver, void *privdata) {
	block_dev_t *bdev;
	node_t *node;
	struct nas *nas;
	struct node_fi *node_fi;

	bdev = (block_dev_t *) pool_alloc(&blockdev_pool);
	if (NULL == bdev) {
		return NULL;
	}

	memset(bdev, 0, sizeof(block_dev_t));

	bdev->id = (dev_t) index_alloc(&block_dev_idx, INDEX_ALLOC_MIN);
	if (-1 == bdev->id) {
		pool_free(&blockdev_pool, bdev);
		return NULL;
	}

	devtab[bdev->id] = bdev;

	bdev->driver = driver;
	bdev->privdata = privdata;

	if (NULL == (node = vfs_create(NULL, path, S_IFBLK | S_IRALL | S_IWALL))) {
		pool_free(&blockdev_pool, bdev);
		index_free(&block_dev_idx, bdev->id);
		return NULL;
	}

	strncpy (bdev->name, node->name, NAME_MAX);
	bdev->dev_node = node;

	nas = node->nas;
	nas->fs = blockdev_fs;
	node_fi = nas->fi;
	node_fi->privdata = bdev;
	node_fi->ni.size = 0;/*TODO*/
	node_fi->ni.mtime = 0;/*TODO*/

	return bdev;
}

int block_dev_read(void *dev, char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *bdev;

	if (NULL == dev) {
		return -ENODEV;
	}
	bdev = block_dev(dev);
	if (NULL == bdev->driver->read) {
		return -ENOSYS;
	}

	return bdev->driver->read(bdev, buffer, count, blkno);
}

int block_dev_write(void *dev, char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *bdev;

	if (NULL == dev) {
		return -ENODEV;
	}
	bdev = block_dev(dev);
	if (NULL == bdev->driver->write) {
		return -ENOSYS;
	}

	return bdev->driver->write(bdev, buffer, count, blkno);
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
	if(NULL == (cache = bdev->cache = pool_alloc(&cache_pool))) {
		return NULL;
	}

	cache->lastblkno = -1;
	cache->buff_cntr = -1;

	if(0 >= (cache->blksize =
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

	if(NULL == (cache->pool = page_alloc(__phymem_allocator, blocks * pagecnt))) {
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

	page_free(__phymem_allocator, cache->pool, cache->depth * cache->blkfactor);
	pool_free(&cache_pool, cache);

	return  0;
}

int block_dev_destroy (void *dev) {
	block_dev_t *bdev;

	bdev = block_dev(dev);
	block_dev_cache_free(bdev);
	index_free(&block_dev_idx, bdev->id);

	pool_free(&blockdev_pool, bdev);
	return 0;
}


ARRAY_SPREAD_DEF(const block_dev_module_t, __block_dev_registry);


int block_devs_init(void) {
	int i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__block_dev_registry); i++) {
		if (NULL != __block_dev_registry[i].init) {
			__block_dev_registry[i].init(NULL);
		}
	}
	return i;
}
