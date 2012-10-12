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
#include <mem/misc/pool.h>
#include <util/array.h>

/*ARRAY_SPREAD_DEF(const block_dev_module_t, __block_dev_registry);*/

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)

POOL_DEF(blockdev_pool, struct block_dev, MAX_DEV_QUANTITY);

unsigned int num_devs = 0;
block_dev_t *devtab[64];

/*
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
*/

block_dev_t *device(dev_t devno) {
	if (devno < 0 || devno >= num_devs) {
		return NULL;
	}
	return devtab[devno];
}

int dev_destroy (dev_t devno) {
	block_dev_t *dev;

	if(NULL ==(dev = device(devno))) {
		return NODEV;
	}
	else {
		free_buffer_pool(devno);

		pool_free(&blockdev_pool, dev);
		devtab[devno] = NULL;
		num_devs--;
		return 0;
	}
}

dev_t dev_make(char *name, block_dev_driver_t *driver, void *privdata) {
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

dev_t devno(char *name) {
	dev_t devno;

	for (devno = 0; devno < num_devs; devno++) {
		if (strcmp(device(devno)->name, name) == 0) {
			return devno;
		}
	}
	return NODEV;
}

dev_t dev_open(char *name) {
	dev_t d = devno(name);
	if (d != NODEV) {
	  devtab[d]->refcnt++;
	}
	return d;
}

int dev_close(dev_t devno) {
	if(devno < 0 || devno >= num_devs) {
		return -ENODEV;
	}
	if(device(devno)->refcnt == 0) {
		return -EPERM;
	}
	device(devno)->refcnt--;
	return 0;
}


int dev_read(dev_t devno, char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *dev;

	if (devno < 0 || devno >= num_devs) {
		return -ENODEV;
	}
	dev = device(devno);
	if (!dev->driver->read) {
		return -ENOSYS;
	}
	dev->reads++;
	dev->input += count;

	return dev->driver->read(dev, buffer, count, blkno);
}

int dev_write(dev_t devno, char *buffer, size_t count, blkno_t blkno) {
	block_dev_t *dev;

	if (devno < 0 || devno >= num_devs) {
		return -ENODEV;
	}
	dev = device(devno);
	if (!dev->driver->write) {
		return -ENOSYS;
	}
	dev->writes++;
	dev->output += count;

	return dev->driver->write(dev, buffer, count, blkno);
}

int dev_ioctl(dev_t devno, int cmd, void *args, size_t size) {
	block_dev_t *dev;

	if (devno < 0 || devno >= num_devs) {
		return -ENODEV;
	}
	dev = device(devno);
	if (!dev->driver->ioctl) {
		return -ENOSYS;
	}

	return dev->driver->ioctl(dev, cmd, args, size);
}
