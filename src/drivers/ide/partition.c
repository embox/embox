/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Andrey Gazukin
 */

#include <asm/io.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <net/in.h>
#include <fs/fat.h>
#include <drivers/ide.h>
#include <embox/block_dev.h>
#include <mem/phymem.h>

static int part_ioctl(block_dev_t *dev, int cmd, void *args, size_t size) {
	struct partition *part = (struct partition *) dev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return part->len;

	case IOCTL_GETBLKSIZE:
		return block_dev_ioctl(part->dev, IOCTL_GETBLKSIZE, NULL, 0);
	}

	return -ENOSYS;
}

static int part_read(block_dev_t *dev, char *buffer,
						size_t count, blkno_t blkno) {
	struct partition *part = (struct partition *) dev->privdata;
	if (blkno + count / SECTOR_SIZE > part->len) {
		return -EFAULT;
	}
	return block_dev_read(part->dev, buffer, count, blkno + part->start);
}

static int part_write(block_dev_t *dev, char *buffer,
						size_t count, blkno_t blkno) {
  struct partition *part = (struct partition *) dev->privdata;
  if (blkno + count / SECTOR_SIZE > part->len) {
	  return -EFAULT;
  }
  return block_dev_write(part->dev, buffer, count, blkno + part->start);
}

static block_dev_driver_t _partition_driver = {
	"partition_drv",
	DEV_TYPE_BLOCK,
	part_ioctl,
	part_read,
	part_write
};

void *partition_driver(void) {
	return &_partition_driver;
}
