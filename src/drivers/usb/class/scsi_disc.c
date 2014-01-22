/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.01.2014
 */

#include <errno.h>
#include <kernel/sched/waitq.h>
#include "scsi.h"

#include <embox/block_dev.h>
#include <util/indexator.h>

static void scsi_user_input(struct scsi_dev *dev, int res) {

	if (res) {
		scsi_dev_recover(dev);
		return;
	}

	dev->cmd_complete = 1;
	waitq_wakeup_all(&dev->wq);
}

static void scsi_user_enter(struct scsi_dev *dev) {
	mutex_init(&dev->m);
	waitq_init(&dev->wq);
}

static const struct scsi_dev_state scsi_state_user = {
	.sds_enter = scsi_user_enter,
	.sds_input = scsi_user_input,
};

static int scsi_wait_cmd_complete(struct scsi_dev *dev, struct scsi_cmd *cmd) {

	/* guards to send only one command */
	if (!dev->in_cmd) {
		dev->in_cmd = 1;
		scsi_do_cmd(dev, cmd);
	}

	if (dev->cmd_complete) {
		dev->cmd_complete = dev->in_cmd = 0;
		return 1;
	}

	return 0;
}

static int scsi_read(block_dev_t *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	struct scsi_dev *sdev = bdev->privdata;
	int blksize = sdev->blk_size;
	unsigned int lba;
	char *bp;
	int ret;

	struct scsi_cmd cmd = scsi_cmd_template_read10;
	cmd.scmd_olen = blksize;

	mutex_lock(&sdev->m);

	for (lba = blkno, bp = buffer;
			count >= blksize;
			lba++, count -= blksize, bp += blksize) {

		cmd.scmd_lba = lba;
		cmd.scmd_obuf = bp;

		ret = WAITQ_WAIT(&sdev->wq, scsi_wait_cmd_complete(sdev, &cmd));
		if (ret) {
			break;
		}

	}
	mutex_unlock(&sdev->m);

	if (ret && bp == buffer) {
		return ret;
	}
	return bp - buffer;
}

static int scsi_write(block_dev_t *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	return -ENOSYS;
}

static int scsi_ioctl(block_dev_t *bdev, int cmd, void *args, size_t size) {
	struct scsi_dev *sdev = bdev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return sdev->blk_n;
	case IOCTL_GETBLKSIZE:
		return sdev->blk_size;
	}

	return -ENOSYS;
}

static const block_dev_driver_t bdev_driver_scsi = {
	"scsi disc",
	scsi_ioctl,
	scsi_read,
	scsi_write
};

INDEX_DEF(scsi_disc_idx, 0, 26);

void scsi_disc_found(struct scsi_dev *sdev) {
	struct block_dev *bdev;
	char path[PATH_MAX];

	strcpy(path, "/dev/sd*");

	if (0 > block_dev_named(path, &scsi_disc_idx)) {
		return;
	}

	bdev = block_dev_create(path, (void *) &bdev_driver_scsi, sdev);
	bdev->size = sdev->blk_n * sdev->blk_size;

	scsi_state_transit(sdev, &scsi_state_user);
}

