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

#include <fs/fat.h>
#include <drivers/ide.h>
#include <embox/block_dev.h>
#include <mem/phymem.h>
#include <util/indexator.h>

static int part_ioctl(block_dev_t *bdev, int cmd, void *args, size_t size) {
	struct partition *part = (struct partition *) bdev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return part->len;

	case IOCTL_GETBLKSIZE:
		return block_dev_ioctl(part->bdev, IOCTL_GETBLKSIZE, NULL, 0);
	}

	return -ENOSYS;
}

static int part_read(block_dev_t *bdev, char *buffer,
						size_t count, blkno_t blkno) {
	struct partition *part = (struct partition *) bdev->privdata;
	if (blkno + count / SECTOR_SIZE > part->len) {
		return -EFAULT;
	}
	return block_dev_read(part->bdev, buffer, count, blkno + part->start);
}

static int part_write(block_dev_t *bdev, char *buffer,
						size_t count, blkno_t blkno) {
	struct partition *part = (struct partition *) bdev->privdata;
	if (blkno + count / SECTOR_SIZE > part->len) {
		return -EFAULT;
	}
	return block_dev_write(part->bdev, buffer, count, blkno + part->start);
}

static block_dev_driver_t partition_driver = {
	"partition_drv",
	part_ioctl,
	part_read,
	part_write
};

block_dev_driver_t *bdev_driver_part = &partition_driver;

/* TODO Create Partition as drive */
int create_partitions(hd_t *hd) {
	mbr_t mbrdata;
	mbr_t *mbr = &mbrdata;
	int rc;

	/* Read partition table */
	rc = block_dev_read(hd->bdev, (char *)mbr, SECTOR_SIZE, 0);
	if (rc < 0) {
		return rc;
	}

	mbr->sig_55 = 0;

	/* Create partition devices */
	if ((mbr->sig_55 != 0x55) || (mbr->sig_aa != 0xAA)) {
		return -EIO;
	}
	/*
	for (i = 0; i < HD_PARTITIONS; i++) {
		hd->parts[i].bdev = hd->devno;
		hd->parts[i].bootid = mbr->ptable[i].active;
		hd->parts[i].systid = mbr->ptable[i].type;
		hd->parts[i].start = mbr->ptable[i].relsect;
		hd->parts[i].len = mbr->ptable[i].numsect;

		if (mbr->parttab[i].systid != 0) {
			sprintf(devname, "%s%c", ((block_dev_module_t *)
			&(__block_dev_registry[hd->devno]))->name, 'a' + i);
			devno = dev_open(devname);
			if (devno == NODEV) {
				devno = dev_make(devname, &partition_driver, NULL, &hd->parts[i]);
			} else {
				dev_close(devno);
			}
		}
	}
	*/
	hd->bdev = block_dev_create("/dev/hda0", &partition_driver, NULL);

	return 0;
}
