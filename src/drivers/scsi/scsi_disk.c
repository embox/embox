/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.01.2014
 */
#include <util/log.h>

#include <endian.h>
#include <errno.h>
#include <string.h>

#include <util/math.h>

#include <drivers/scsi.h>

#include <drivers/block_dev.h>

static void scsi_disk_lock(struct block_dev *bdev) {
	struct scsi_dev *sdev = block_dev_priv(bdev);

	scsi_dev_use_inc(sdev);
	mutex_lock(&sdev->m);
}

static void scsi_disk_unlock(struct block_dev *bdev) {
	struct scsi_dev *sdev = block_dev_priv(bdev);

	if (sdev->use_count == 1) {
		scsi_disk_bdev_try_unbind(sdev);
	}

	mutex_unlock(&sdev->m);
	scsi_dev_use_dec(sdev);
}

static int scsi_read_blocks(struct scsi_dev *sdev,
		int blk_start, int blk_count, uint8_t *buf) {
	struct scsi_cmd cmd;
	struct scsi_cmd_read10 cmd_read10;

	memset(&cmd_read10, 0, sizeof cmd_read10);
	cmd_read10.sr10_opcode = SCSI_CMD_OPCODE_READ10;
	cmd_read10.sr10_lba = htobe32(blk_start);
	cmd_read10.sr10_transfer_len = htobe16(blk_count);

	cmd.scmd_buf    = (uint8_t *) &cmd_read10;
	cmd.scmd_len    = sizeof cmd_read10;
	cmd.sdata_buf   = buf;
	cmd.sdata_len   = blk_count * sdev->blk_size;

	return scsi_do_cmd(sdev, &cmd);
}

static int scsi_write_blocks(struct scsi_dev *sdev,
		int blk_start, int blk_count, uint8_t *buf) {
	struct scsi_cmd cmd;
	struct scsi_cmd_write10 cmd_write10;

	memset(&cmd_write10, 0, sizeof cmd_write10);
	cmd_write10.sw10_opcode = SCSI_CMD_OPCODE_WRITE10;
	cmd_write10.sw10_lba = htobe32(blk_start);
	cmd_write10.sw10_transfer_len = htobe16(blk_count);

	cmd.scmd_buf    = (uint8_t *) &cmd_write10;
	cmd.scmd_len    = sizeof cmd_write10;
	cmd.sdata_buf   = buf;
	cmd.sdata_len   = blk_count * sdev->blk_size;

	return scsi_do_cmd(sdev, &cmd);
}

static int scsi_read(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	struct scsi_dev *sdev = block_dev_priv(bdev);
	int blksize = sdev->blk_size;
	unsigned int lba;
	char *bp;
	int ret;

	if (!sdev) {
		return -ENODEV;
	}

	scsi_disk_lock(bdev);
	for (lba = blkno, bp = buffer;
			count >= blksize;
			lba++, count -= blksize, bp += blksize) {
		ret = scsi_read_blocks(sdev, lba, 1, (uint8_t *) bp);
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

	sdev = block_dev_priv(bdev);
	blksize = sdev->blk_size;

	if (!sdev) {
		return -ENODEV;
	}

	scsi_disk_lock(bdev);
	for (lba = blkno, bp = buffer;
			count >= blksize;
			lba++, count -= blksize, bp += blksize) {
		ret = scsi_write_blocks(sdev, lba, 1, (uint8_t *) bp);
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
	struct scsi_dev *sdev = block_dev_priv(bdev);
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

const struct block_dev_ops bdev_driver_scsi = {
	"scsi disk",
	scsi_ioctl,
	scsi_read,
	scsi_write
};
