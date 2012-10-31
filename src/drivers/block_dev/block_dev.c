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

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)

POOL_DEF(blockdev_pool, struct block_dev, MAX_DEV_QUANTITY);

POOL_DEF(cache_pool, struct _cache_pool, MAX_DEV_QUANTITY);

static unsigned int num_devs = 0;
block_dev_t *devtab[64];


block_dev_module_t *block_dev_find(char *name) {
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

block_dev_t *block_dev(dev_t devno) {
	if (devno < 0 || devno >= num_devs) {
		return NULL;
	}
	return devtab[devno];
}

dev_t devno(char *name) {
	dev_t devno;

	for (devno = 0; devno < num_devs; devno++) {
		if (strcmp(block_dev(devno)->name, name) == 0) {
			return devno;
		}
	}
	return NODEV;
}

int block_dev_destroy (dev_t devno) {
	block_dev_t *dev;

	if(NULL ==(dev = block_dev(devno))) {
		return NODEV;
	}
	else {
		free_cache_pool(devno);

		pool_free(&blockdev_pool, dev);
		devtab[devno] = NULL;
		num_devs--;
		return 0;
	}
}

dev_t block_dev_make(char *name, block_dev_driver_t *driver, void *privdata) {
	block_dev_t *dev;
	dev_t devno;
	char *p;
	unsigned int n, m;
	int exists;

	if (num_devs >= MAX_DEV_QUANTITY) {
	  return NODEV;
	}

	dev = (block_dev_t *) pool_alloc(&blockdev_pool);
	if (!dev) {
		return NODEV;
	}
	memset(dev, 0, sizeof(block_dev_t));

	strcpy(dev->name, name);

	p = dev->name;
	while (p[0] && p[1]) {
		p++;
	}
	if (*p == '#') {
		n = 0;
		while (1) {
			sprintf(p, "%d", n);
			exists = 0;
			for (m = 0; m < num_devs; m++)  {
				if (strcmp(devtab[m]->name, dev->name) == 0) {
					exists = 1;
					break;
				}
			}

			if (!exists) {
				break;
			}
			n++;
		}
	}

	dev->driver = driver;
	dev->privdata = privdata;
	dev->refcnt = 0;
	dev->mode = 0600;

	/*  switch (dev->driver->type) {
		case DEV_TYPE_STREAM: dev->mode |= S_IFCHR; break;
		case DEV_TYPE_BLOCK: dev->mode |= S_IFBLK; break;
		case DEV_TYPE_PACKET: dev->mode |= S_IFPKT; break;
	}
	*/

	devno = num_devs++;
	devtab[devno] = dev;

	return devno;
}

dev_t block_dev_open(char *name) {
	dev_t d = devno(name);
	if (d != NODEV) {
	  devtab[d]->refcnt++;
	}
	return d;
}

int block_dev_close(dev_t devno) {
	if(devno < 0 || devno >= num_devs) {
		return -ENODEV;
	}
	if(block_dev(devno)->refcnt == 0) {
		return -EPERM;
	}
	block_dev(devno)->refcnt--;
	return 0;
}


int block_dev_read(dev_t devno, char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *dev;

	if (devno < 0 || devno >= num_devs) {
		return -ENODEV;
	}
	dev = block_dev(devno);
	if (!dev->driver->read) {
		return -ENOSYS;
	}
	dev->reads++;
	dev->input += count;

	return dev->driver->read(dev, buffer, count, blkno);
}

int block_dev_write(dev_t devno, char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *dev;

	if (devno < 0 || devno >= num_devs) {
		return -ENODEV;
	}
	dev = block_dev(devno);
	if (!dev->driver->write) {
		return -ENOSYS;
	}
	dev->writes++;
	dev->output += count;

	return dev->driver->write(dev, buffer, count, blkno);
}

int block_dev_ioctl(dev_t devno, int cmd, void *args, size_t size) {
	block_dev_t *dev;

	if (devno < 0 || devno >= num_devs) {
		return -ENODEV;
	}
	dev = block_dev(devno);
	if (!dev->driver->ioctl) {
		return -ENOSYS;
	}

	return dev->driver->ioctl(dev, cmd, args, size);
}

cache_pool_t *init_cache_pool(dev_t devno, int blocks) {
	int pagecnt;
	block_dev_t *dev;
	cache_pool_t *cache;

	if(NULL == (dev = block_dev(devno))) {
		return NULL;
	}

	if (NULL == dev->cache) {
		cache = (cache_pool_t *) pool_alloc(&cache_pool);
		dev->cache = cache;
	}
	else {
		cache = dev->cache;
	}
	cache->lastblkno = -1;
	cache->buff_cntr = -1;

	if(0 >= (cache->blksize =
			block_dev_ioctl(devno, IOCTL_GETBLKSIZE, NULL, 0))) {
		return NULL;
	}

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

cache_pool_t *get_cached_block(dev_t devno, blkno_t blkno) {
	cache_pool_t *cache;

	if(NULL == (cache = (block_dev(devno)->cache))) {
		return NULL;
	}

	/* set pointer to the buffer in pool */
	if(cache->lastblkno != blkno) {
		cache->buff_cntr++;
		cache->buff_cntr %= cache->depth;

		cache->data = cache->pool + cache->buff_cntr * PAGE_SIZE() * cache->blkfactor;
		cache->blkno = blkno;

		block_dev_read(devno, cache->data, cache->blksize, cache->blkno);
		cache->lastblkno = blkno;
	}

	return cache;
}

int free_cache_pool(dev_t devno) {
	block_dev_t *dev;
	cache_pool_t *cache;

	if(NULL == (dev = block_dev(devno))) {
		return -1;
	}

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

