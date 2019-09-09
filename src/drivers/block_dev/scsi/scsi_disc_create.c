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

#include <util/indexator.h>

INDEX_DEF(scsi_disk_idx, 0, 26);

extern const struct block_dev_driver bdev_driver_scsi;


static void scsi_user_input(struct scsi_dev *dev, int res) {
	if (res) {
		scsi_dev_recover(dev);
		return;
	}

	scsi_dev_wake(dev, 1);
}

static void scsi_user_enter(struct scsi_dev *dev) {
	mutex_init(&dev->m);
	waitq_init(&dev->wq);
}

static const struct scsi_dev_state scsi_state_user = {
	.sds_enter = scsi_user_enter,
	.sds_input = scsi_user_input,
};

static void* scsi_create_thread(void *arg) {
	struct block_dev *bdev;
	struct scsi_dev *sdev;
	char path[0x10]; /* enough for /dev/sdx */

	assert(arg);
	sdev = arg;

	strcpy(path, "/dev/sd*");

	if (0 > (sdev->idx = block_dev_named(path, &scsi_disk_idx))) {
		return NULL;
	}

	bdev = block_dev_create(path, (void *) &bdev_driver_scsi, NULL);
	if (!bdev) {
		log_error("can't create block device");
		return NULL;
	}

	bdev->privdata = sdev;
	bdev->size = sdev->blk_n * sdev->blk_size;
	sdev->bdev = bdev;
	create_partitions(bdev);

	return 0;
}

void scsi_disk_found(struct scsi_dev *sdev) {
	scsi_state_transit(sdev, &scsi_state_user);
	thread_create(THREAD_FLAG_DETACHED, scsi_create_thread, sdev);
}

void scsi_disk_lost(struct scsi_dev *sdev) {

	scsi_dev_wake(sdev, -ENODEV);

	if (!sdev->use_count) {
		scsi_disk_bdev_try_unbind(sdev);
	}
}

void scsi_disk_bdev_try_unbind(struct scsi_dev *sdev) {
	if (!sdev->attached) {
		sdev->bdev->privdata = NULL;
		index_free(&scsi_disk_idx, sdev->idx);
		block_dev_destroy(sdev->bdev);
	}
}

void scsi_dev_wake(struct scsi_dev *dev, int res) {
	dev->cmd_complete = res;
	waitq_wakeup_all(&dev->wq);
}

