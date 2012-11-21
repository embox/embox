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
#include <mem/phymem.h>
#include <mem/misc/pool.h>
#include <util/array.h>
#include <util/indexator.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)

POOL_DEF(blockdev_pool, struct block_dev, MAX_DEV_QUANTITY);
POOL_DEF(cache_pool, struct block_dev_cache, MAX_DEV_QUANTITY);
INDEX_DEF(block_dev_idx,0,MAX_DEV_QUANTITY);

static struct block_dev *devtab[MAX_DEV_QUANTITY];

static int block_dev_cache_free(void *dev_id);

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

block_dev_t *block_dev(void *dev_id) {
	dev_t devno;
	block_dev_t *dev;

	assert(dev_id != NULL);
	devno = *((dev_t *) dev_id);
	dev = devtab[devno];
	assert(dev != NULL);

	return dev;
}

#define WHOLE_NAME     0
#define DIGITAL_IDX    1
#define CHARACTER_IDX  2
static char check_type_name (char *p) {
	if (*p == '#') {
		return DIGITAL_IDX;
	}
	else if(*p == '*') {
		return CHARACTER_IDX;
	}
	/* specifies the full name of the device */
	else {
		return WHOLE_NAME;
	}
}

static void block_dev_select_name(char *name, dev_t *idx) {
	char *p;

	p = name;
	if (*p) {
	    p += strlen(p+1);
	}
	else {
		return;
	}

	if(NULL != idx){
		switch (check_type_name (p)) {
		case WHOLE_NAME:
			break;

		case DIGITAL_IDX:
			sprintf(p, "%d", *idx);
			break;

		case CHARACTER_IDX:
			sprintf(p, "%c", 'a' + *idx);
			break;
		}
	}
	return;
}

static int block_dev_create_node(dev_t *dev_id, char *dev_path) {
	node_t *dev_node;

	strcat(dev_path, block_dev(dev_id)->name);

	if ((NULL == (dev_node = vfs_add_path(dev_path, NULL))) ||
	    (NULL == (dev_node = vfs_find_node(dev_path, NULL)))) {
		return NODEV;
	}

	dev_node->dev_id = (void *) dev_id;
	block_dev(dev_id)->dev_node = dev_node;

	return 0;
}

dev_t *block_dev_create(char *path, void *driver, void *privdata, dev_t *name_idx) {
	block_dev_t *dev;
	char name[MAX_LENGTH_FILE_NAME];

	dev = (block_dev_t *) pool_alloc(&blockdev_pool);
	if (NULL == dev) {
		return NULL;
	}
	memset(dev, 0, sizeof(block_dev_t));

	dev->id = (dev_t) index_alloc(&block_dev_idx, INDEX_ALLOC_MIN);
	if(-1 == dev->id) {
		pool_free(&blockdev_pool, dev);
		return NULL;
	}

	devtab[dev->id] = dev;

	*name = 0;
	path_nip_tail(path, name);
	block_dev_select_name(name, name_idx);
	strncpy (dev->name, name, MAX_LENGTH_FILE_NAME);

	dev->driver = driver;
	dev->privdata = privdata;

	if(-1 == block_dev_create_node(&dev->id, path)) {
		pool_free(&blockdev_pool, dev);
		index_free(&block_dev_idx, dev->id);
		return NULL;
	}
	return &dev->id;
}

int block_dev_read(void *dev_id, char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *dev;

	if (NULL == dev_id) {
		return -ENODEV;
	}
	dev = block_dev(dev_id);
	if (NULL == dev->driver->read) {
		return -ENOSYS;
	}

	return dev->driver->read(dev, buffer, count, blkno);
}

int block_dev_write(void *dev_id, char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *dev;

	if (NULL == dev_id) {
		return -ENODEV;
	}
	dev = block_dev(dev_id);
	if (NULL == dev->driver->write) {
		return -ENOSYS;
	}

	return dev->driver->write(dev, buffer, count, blkno);
}

int block_dev_ioctl(void *dev_id, int cmd, void *args, size_t size) {
	block_dev_t *dev;

	if (NULL == dev_id) {
		return -ENODEV;
	}
	dev = block_dev(dev_id);

	if (NULL == dev->driver->ioctl) {
		return -ENOSYS;
	}

	return dev->driver->ioctl(dev, cmd, args, size);
}


block_dev_cache_t *block_dev_cache_init(void *dev_id, int blocks) {
	int pagecnt;
	block_dev_cache_t *cache;
	block_dev_t *dev;

	if (NULL == dev_id) {
		return NULL;
	}
	/* if reinit */
	block_dev_cache_free(dev_id);

	dev = block_dev(dev_id);
	if(NULL == (cache = dev->cache = pool_alloc(&cache_pool))) {
		return NULL;
	}

	cache->lastblkno = -1;
	cache->buff_cntr = -1;

	if(0 >= (cache->blksize =
			block_dev_ioctl(dev, IOCTL_GETBLKSIZE, NULL, 0))) {
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

block_dev_cache_t *block_dev_cached_read(void *dev_id, blkno_t blkno) {
	block_dev_cache_t *cache;
	block_dev_t *dev;

	if (NULL == dev_id) {
		return NULL;
	}
	dev = block_dev(dev_id);

	if(NULL == (cache = dev->cache)) {
		return NULL;
	}

	/* set pointer to the buffer in pool */
	if(cache->lastblkno != blkno) {
		cache->buff_cntr++;
		cache->buff_cntr %= cache->depth;

		cache->data = cache->pool + cache->buff_cntr * PAGE_SIZE() * cache->blkfactor;
		cache->blkno = blkno;

		block_dev_read(dev, cache->data, cache->blksize, cache->blkno);
		cache->lastblkno = blkno;
	}

	return cache;
}

static int block_dev_cache_free(void *dev_id) {
	block_dev_t *dev;
	block_dev_cache_t *cache;

	if (NULL == dev_id) {
		return -1;
	}
	dev = block_dev(dev_id);

	if (NULL == dev->cache) {
		return 0;
	}
	else {
		cache = dev->cache;
	}

	page_free(__phymem_allocator, cache->pool, cache->depth * cache->blkfactor);
	pool_free(&cache_pool, cache);

	return  0;
}

int block_dev_destroy (void *dev_id) {
	block_dev_t *dev;

	dev = block_dev(dev_id);
	block_dev_cache_free(dev);
	index_free(&block_dev_idx, dev->id);

	pool_free(&blockdev_pool, dev);
	return 0;
}
