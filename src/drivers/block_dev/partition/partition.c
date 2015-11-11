/**
 * @file
 * @brief
 *
 * @date 29.10.2012
 * @author Andrey Gazukin
 */

#include <asm/io.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <fs/mbr.h>
#include <drivers/ide.h>
#include <drivers/block_dev.h>
#include <mem/phymem.h>

static int part_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size) {
	struct partition *part = (struct partition *) bdev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return part->len;

	case IOCTL_GETBLKSIZE:
		return block_dev_ioctl(part->bdev, IOCTL_GETBLKSIZE, NULL, 0);
	}

	return -ENOSYS;
}

static int part_read(struct block_dev *bdev, char *buffer,
						size_t count, blkno_t blkno) {
	struct partition *part = (struct partition *) bdev->privdata;
	if (blkno + count / bdev->block_size > part->len) {
		return -EFAULT;
	}
	return block_dev_read(part->bdev, buffer, count, blkno + part->start);
}

static int part_write(struct block_dev *bdev, char *buffer,
						size_t count, blkno_t blkno) {
	struct partition *part = (struct partition *) bdev->privdata;
	if (blkno + count / bdev->block_size > part->len) {
		return -EFAULT;
	}
	return block_dev_write(part->bdev, buffer, count, blkno + part->start);
}

static struct block_dev_driver partition_driver = {
	"partition_drv",
	part_ioctl,
	part_read,
	part_write
};

struct block_dev_driver *bdev_driver_part = &partition_driver;

/* TODO Create Partition as drive */
int create_partitions(struct block_dev *bdev) {
	struct mbr mbr;
	int rc;
	int part_n;
	char part_name[0x16];

	/* Read partition table */
	rc = block_dev_read(bdev, (char *)&mbr, bdev->block_size, 0);
	if (rc < 0) {
		return rc;
	}

	/* Create partition devices */
	if ((mbr.sig_55 != 0x55) || (mbr.sig_aa != 0xAA)) {
		return 0;
	}
	for (part_n = 0; part_n < 4; part_n ++) {

		if(mbr.ptable[part_n].type == 0) {
			return part_n;
		}
		sprintf(part_name, "%s/%s%d", "/dev/", bdev->name, part_n);
		block_dev_create(part_name, &partition_driver, NULL);
	}

	return 0;
}
