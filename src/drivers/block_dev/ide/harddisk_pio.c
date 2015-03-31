/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <asm/io.h>

#include <kernel/irq_lock.h>
#include <drivers/ide.h>
#include <embox/block_dev.h>
#include <mem/phymem.h>
#include <util/indexator.h>
#include <kernel/time/ktime.h>
#include <limits.h>
#include <kernel/thread/waitq.h>

#define HD_WAIT_MS 10

extern int hd_ioctl(block_dev_t *bdev, int cmd, void *args, size_t size);

static int hd_read_pio(block_dev_t *bdev, char *buffer, size_t count, blkno_t blkno) {
	hd_t *hd;
	hdc_t *hdc;
	int sectsleft;
	int nsects;
	int result = 0;
	char *bufp;

	if (count == 0) {
		return 0;
	}
	bufp = (char *) buffer;
	hd = (hd_t *) bdev->privdata;
	hdc = hd->hdc;
	sectsleft = count / SECTOR_SIZE;
	if (count % SECTOR_SIZE) {
		sectsleft++;
	}

	while (sectsleft > 0) {
		/* Select drive */
		ide_select_drive(hd);

		/* Wait for controller ready */
		result = ide_wait(hdc, HDCS_DRDY, HDTIMEOUT_DRDY);
		if (result != 0) {
			hdc->result = -EIO;
			break;
		}

#if 0
		/* Calculate maximum number of sectors we can transfer */
		if (sectsleft > 256) {
			nsects = 256;
		} else {
			nsects = sectsleft;
		}
#else
		nsects = 1;
#endif

		/* Prepare transfer */
		hdc->bufp = bufp;
		hdc->nsects = nsects;
		hdc->result = 0;
		hdc->dir = HD_XFER_READ;
		hdc->active = hd;

		hd_setup_transfer(hd, blkno, nsects);
		outb(hd->multsect > 1 ? HDCMD_MULTREAD : HDCMD_READ,
				hdc->iobase + HDC_COMMAND);

		/* Wait until data read */
		WAITQ_WAIT(&hdc->waitq, hdc->result);

		if (hdc->result < 0) {
			break;
		}

		/* Advance to next */
		sectsleft -= nsects;
		bufp += nsects * SECTOR_SIZE;
		blkno += nsects; /*WTF?*/
	}

	/* Cleanup */
	hdc->dir = HD_XFER_IDLE;
	hdc->active = NULL;
	if (0 < hdc->result) {
		result = hdc->result = 0;
	}

	return result == 0 ? count : result;
}

static int hd_write_pio(block_dev_t *bdev, char *buffer, size_t count, blkno_t blkno) {
	hd_t *hd;
	hdc_t *hdc;
	int sectsleft;
	int nsects;
	int n;
	int result = 0;
	char *bufp;

	if (count == 0) {
		return 0;
	}
	bufp = (char *) buffer;
	hd = (hd_t *) bdev->privdata;
	hdc = hd->hdc;
	sectsleft = count / SECTOR_SIZE;


	while (sectsleft > 0) {
		/* Select drive */
		ide_select_drive(hd);

		/* Wait for controller ready */
		result = ide_wait(hdc, HDCS_DRDY, HDTIMEOUT_DRDY);
		if (result != 0) {
			hdc->result = -EIO;
			break;
		}
#if 0
		/* Calculate maximum number of sectors we can transfer */
		if (sectsleft > 256) {
			nsects = 256;
		} else {
			nsects = sectsleft;
		}
#else
		nsects = 1;
#endif

		/* Prepare transfer */
		hdc->bufp = bufp;
		hdc->nsects = nsects;
		hdc->result = 0;
		hdc->dir = HD_XFER_WRITE;
		hdc->active = hd;

		hd_setup_transfer(hd, blkno, nsects);
		outb(hd->multsect > 1 ? HDCMD_MULTWRITE : HDCMD_WRITE,
				hdc->iobase + HDC_COMMAND);

		/* Wait for data ready */
		if (!(inb(hdc->iobase + HDC_ALT_STATUS) & HDCS_DRQ)) {
			result = ide_wait(hdc, HDCS_DRQ, HDTIMEOUT_DRQ);
			if (result != 0) {
				hdc->result = -EIO;
				break;
			}
		}

		/* Write first sector(s) */
		n = hd->multsect;
		if (n > nsects) {
			n = nsects;
		}
		while (n-- > 0) {
			pio_write_buffer(hd, hdc->bufp, SECTOR_SIZE);
			hdc->bufp += SECTOR_SIZE;
		}

		/* Wait until data written */
		WAITQ_WAIT(&hdc->waitq, hdc->result);

		if (hdc->result < 0) {
			break;
		}

		/* Advance to next */
		sectsleft -= nsects;
		bufp += nsects * SECTOR_SIZE;
		blkno += nsects; /*WTF?*/
	}

	/* Cleanup */
	hdc->dir = HD_XFER_IDLE;
	hdc->active = NULL;

	/*
	 * FIXME This assignment crashes writing, because
	 * hdc->result equal 1 after writing. So, it set result to 1, afterward
	 * (return result == 0 ? count : result) return 1.
	 * --Alexander
	 *
	result = hdc->result;
	*/

	return result == 0 ? count : result;
}


static block_dev_driver_t idedisk_pio_driver = {
	"idedisk_drv",
	hd_ioctl,
	hd_read_pio,
	hd_write_pio
};

static int idedisk_init (void *args) {
//	struct ide_tab *ide;
	hd_t *drive;
	size_t size;
	char path[PATH_MAX];
#if 0
	ide = ide_get_drive();

	for(int i = 0; i < HD_DRIVES; i++) {
		if (NULL == ide->drive[i]) {
			continue;
		} else {
			drive = (hd_t *) ide->drive[i];
#endif
			drive = (hd_t *)args;
			/* Make new device */
			if ((drive->media == IDE_DISK) && (drive->udmamode == -1)) {
				*path = 0;
				strcat(path, "/dev/hd*");
				if (0 > (drive->idx = block_dev_named(path, idedisk_idx))) {
					return drive->idx;
				}
				drive->bdev = block_dev_create(path,
						&idedisk_pio_driver, drive);
				if (NULL != drive->bdev) {
					size = drive->blks * SECTOR_SIZE;
					block_dev(drive->bdev)->size = size;
				} else {
					return -1;
				}
				create_partitions(drive);
//			} else {
//				continue;
//			}
		}
//	}
	return 0;
}

EMBOX_BLOCK_DEV("idedisk", &idedisk_pio_driver, idedisk_init);
