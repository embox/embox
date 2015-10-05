/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.01.2014
 */

#include <errno.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread/waitq.h>
#include <util/math.h>
#include <string.h>

#include <drivers/scsi.h>

#include <drivers/block_dev.h>
#include <util/indexator.h>

INDEX_DEF(scsi_disk_idx, 0, 26);

static int scsi_wake_res(struct scsi_dev *dev) {
	return min(dev->cmd_complete, 0);
}

static int scsi_wait_cmd_complete(struct scsi_dev *dev, struct scsi_cmd *cmd) {

	/* guards to send only one command */
	if (!dev->in_cmd) {
		dev->in_cmd = 1;
		dev->cmd_complete = scsi_do_cmd(dev, cmd);
	}

	if (dev->cmd_complete) {
		dev->in_cmd = 0;
		return dev->cmd_complete;
	}

	return 0;
}

static void scsi_wake(struct scsi_dev *dev, int res) {
	dev->cmd_complete = res;
	waitq_wakeup_all(&dev->wq);
}

static void scsi_user_input(struct scsi_dev *dev, int res) {

	if (res) {
		scsi_dev_recover(dev);
		return;
	}

	scsi_wake(dev, 1);
}

static void scsi_user_enter(struct scsi_dev *dev) {
	mutex_init(&dev->m);
	waitq_init(&dev->wq);
}

static const struct scsi_dev_state scsi_state_user = {
	.sds_enter = scsi_user_enter,
	.sds_input = scsi_user_input,
};

static void scsi_disk_bdev_try_unbind(struct scsi_dev *sdev) {
	if (!sdev->attached) {
		sdev->bdev->privdata = NULL;
		index_free(&scsi_disk_idx, sdev->idx);
		block_dev_destroy(sdev->bdev);
	}
}

static void scsi_disk_lock(struct block_dev *bdev) {
	struct scsi_dev *sdev = bdev->privdata;

	scsi_dev_use_inc(sdev);
	mutex_lock(&sdev->m);
}

static void scsi_disk_unlock(struct block_dev *bdev) {
	struct scsi_dev *sdev = bdev->privdata;

	if (sdev->use_count == 1) {
		scsi_disk_bdev_try_unbind(sdev);
	}

	mutex_unlock(&sdev->m);
	scsi_dev_use_dec(sdev);
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

	if (!sdev) {
		return -ENODEV;
	}

	scsi_disk_lock(bdev);
	for (lba = blkno, bp = buffer;
			count >= blksize;
			lba++, count -= blksize, bp += blksize) {

		cmd.scmd_lba = lba;
		cmd.scmd_obuf = bp;

		ret = WAITQ_WAIT(&sdev->wq, scsi_wait_cmd_complete(sdev, &cmd));
		if (!ret) {
			ret = scsi_wake_res(sdev);
		}
		if (ret) {
			break;
		}

	}
	scsi_disk_unlock(bdev);

	if (ret && bp == buffer) {
		return ret;
	}
	return bp - buffer;
}

static int scsi_write(block_dev_t *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	struct scsi_dev *sdev;
	int blksize;
	unsigned int lba;
	char *bp;
	int ret;

	assert(bdev);
	assert(buffer);

	sdev = bdev->privdata;
	blksize = sdev->blk_size;

	struct scsi_cmd cmd = scsi_cmd_template_write10;
	cmd.scmd_olen = blksize;

	if (!sdev) {
		return -ENODEV;
	}

	scsi_disk_lock(bdev);
	for (lba = blkno, bp = buffer;
			count >= blksize;
			lba++, count -= blksize, bp += blksize) {

		cmd.scmd_lba = lba;
		cmd.scmd_obuf = bp;

		ret = WAITQ_WAIT(&sdev->wq, scsi_wait_cmd_complete(sdev, &cmd));
		if (!ret) {
			ret = scsi_wake_res(sdev);
		}
		if (ret) {
			break;
		}

	}
	scsi_disk_unlock(bdev);

	if (ret && bp == buffer) {
		return ret;
	}
	return bp - buffer;
}

static int scsi_ioctl(block_dev_t *bdev, int cmd, void *args, size_t size) {
	struct scsi_dev *sdev = bdev->privdata;
	int ret;

	if (!sdev) {
		return -ENODEV;
	}

	scsi_disk_lock(bdev);
	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		ret = sdev->blk_n;
		break;
	case IOCTL_GETBLKSIZE:
		ret = sdev->blk_size;
		break;
	default:
		ret = -ENOSYS;
	}
	scsi_disk_unlock(bdev);

	return ret;
}

static const block_dev_driver_t bdev_driver_scsi = {
	"scsi disk",
	scsi_ioctl,
	scsi_read,
	scsi_write
};

void scsi_disk_found(struct scsi_dev *sdev) {
	struct block_dev *bdev;
	char path[PATH_MAX];

	strcpy(path, "/dev/sd*");

	if (0 > (sdev->idx = block_dev_named(path, &scsi_disk_idx))) {
		return;
	}

	bdev = block_dev_create(path, (void *) &bdev_driver_scsi, sdev);
	bdev->size = sdev->blk_n * sdev->blk_size;
	sdev->bdev = bdev;

	scsi_state_transit(sdev, &scsi_state_user);
}

void scsi_disk_lost(struct scsi_dev *sdev) {

	scsi_wake(sdev, -ENODEV);

	if (!sdev->use_count) {
		scsi_disk_bdev_try_unbind(sdev);
	}
}
