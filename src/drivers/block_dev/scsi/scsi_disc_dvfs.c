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
#include <drivers/device.h>

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

static int scsi_read(struct block_dev *bdev, char *buffer, size_t count,
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

static int scsi_write(struct block_dev *bdev, char *buffer, size_t count,
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

const struct block_dev_driver bdev_driver_scsi = {
	.name = "scsi disk",
	.read = scsi_read,
	.write = scsi_write
};
