/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.01.2014
 */
#include <util/log.h>

#include <errno.h>
#include <string.h>

#include <util/math.h>

#include <drivers/scsi.h>

#include <drivers/block_dev.h>

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

		ret = scsi_do_cmd(sdev, &cmd);
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

		ret = scsi_do_cmd(sdev, &cmd);
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

static int scsi_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size) {
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

const struct block_dev_driver bdev_driver_scsi = {
	"scsi disk",
	scsi_ioctl,
	scsi_read,
	scsi_write
};
