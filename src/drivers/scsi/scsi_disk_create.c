/**
 * @file
 *
 * @data 10.11.15
 * @author: Anton Bondarev
 */
#include <util/log.h>

#include <errno.h>
#include <limits.h>
#include <string.h>

#include <drivers/scsi.h>
#include <drivers/block_dev.h>
#include <drivers/block_dev/partition.h>

#include <mem/misc/pool.h>
#include <kernel/thread.h>

#include <lib/libds/indexator.h>

INDEX_DEF(scsi_disk_idx, 0, 26);

extern const struct block_dev_ops bdev_driver_scsi;

static void *scsi_create_thread(void *arg) {
	struct block_dev *bdev;
	struct scsi_dev *sdev;
	char path[0x10]; /* enough for /dev/sdx */

	assert(arg);
	sdev = arg;

	mutex_init(&sdev->m);

	strcpy(path, "/dev/sd*");

	if (0 > (sdev->idx = block_dev_named(path, &scsi_disk_idx))) {
		return NULL;
	}

	bdev = block_dev_create(path, (void *) &bdev_driver_scsi, sdev);
	if (!bdev) {
		log_error("can't create block device");
		return NULL;
	}

	bdev->size = (uint64_t)sdev->blk_n * sdev->blk_size;
	sdev->bdev = bdev;

	return 0;
}

void scsi_disk_found(struct scsi_dev *sdev) {
	scsi_create_thread(sdev);
}

void scsi_disk_lost(struct scsi_dev *sdev) {
	if (!sdev->use_count) {
		scsi_disk_bdev_try_unbind(sdev);
	}
}

void scsi_disk_bdev_try_unbind(struct scsi_dev *sdev) {
	if (!sdev->attached) {
		index_free(&scsi_disk_idx, sdev->idx);
		block_dev_destroy(sdev->bdev);
	}
}
