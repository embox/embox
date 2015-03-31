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

#include <kernel/sched/sched_lock.h>
#include <arpa/inet.h>
#include <drivers/ide.h>
#include <embox/block_dev.h>
#include <mem/phymem.h>
#include <util/indexator.h>
#include <kernel/time/ktime.h>
#include <limits.h>

#define CD_WAIT_US 3000

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)
INDEX_DEF(idecd_idx,0,MAX_DEV_QUANTITY);

static int atapi_packet_read(hd_t *hd, unsigned char *pkt,
		                     int pktlen, char *buffer, size_t bufsize) {
	hdc_t *hdc;
	int result;
	char *bufp;
	int bufleft;
	unsigned short bytes;

	hdc = hd->hdc;
	bufp = (char *) buffer;
	bufleft = bufsize;
	hdc->dir = HD_XFER_IGNORE;
	hdc->active = hd;
	hdc->result = 0;

	/* Setup registers */
	outb(0, hdc->iobase + HDC_FEATURE);
	outb(0, hdc->iobase + HDC_SECTORCNT);
	outb(0, hdc->iobase + HDC_SECTOR);
	outb((unsigned char) (bufsize & 0xFF), hdc->iobase + HDC_TRACKLSB);
	outb((unsigned char) (bufsize >> 8), hdc->iobase + HDC_TRACKMSB);
	outb((unsigned char) hd->drvsel, hdc->iobase + HDC_DRVHD);

	/* Send packet command */
	outb(HDCMD_PACKET, hdc->iobase + HDC_COMMAND);

	/* Wait for drive ready to receive packet */
	result = ide_wait(hdc, HDCS_DRDY, HDTIMEOUT_DRDY);
	if (result != 0) {

		hdc->dir = HD_XFER_IDLE;
		hdc->active = NULL;

		return -EIO;
	}

	/* Command packet transfer */
	hdc->result = 0;
	pio_write_buffer(hd, (char *) pkt, pktlen);

	/* Data transfer */
	while (1) {
		/* Wait until data ready */
		usleep(CD_WAIT_US);

		/* Check for errors */
		if (hdc->status & HDCS_ERR) {
			hdc->result = -EIO;
			break;
		}
		/*
		 * Exit the read data loop if the device indicates this is
		 * the end of the command
		 */
		if ((hdc->status & (HDCS_BSY | HDCS_DRQ)) == 0) {
			break;
		}

		/* Get the byte count */
		bytes = (inb(hdc->iobase + HDC_TRACKMSB) << 8) | inb(hdc->iobase + HDC_TRACKLSB);
		if (bytes == 0) {
			break;
		}
		if (bytes > bufleft) {
			hdc->result = -ENOBUFS;
			break;
		}
		/* Read the bytes */
		pio_read_buffer(hd, bufp, bytes);
		bufp += bytes;
		bufleft -= bytes;
	}

	/* Cleanup */
	hdc->dir = HD_XFER_IDLE;
	hdc->active = NULL;
	if (0 < hdc->result) {
		result = hdc->result = 0;
	}
	return result == 0 ? bufsize - bufleft : -EIO;
}

static int atapi_read_capacity(hd_t *hd) {
	unsigned char pkt[12];
	unsigned long buf[2];
	unsigned long blks;
	int rc;

	memset(pkt, 0, 12);
	pkt[0] = ATAPI_CMD_READCAPICITY;

	rc = atapi_packet_read(hd, pkt, 12, (char *)buf, sizeof buf);
	if (rc < 0) {
		return rc;
	}
	if (rc != sizeof buf) {
		return -ENOBUFS;
	}

	blks = ntohl(buf[0]);
	return blks;
}

static int atapi_request_sense(hd_t *hd) {
	unsigned char pkt[12];
	unsigned char buf[18];
	int rc;

	memset(pkt, 0, 12);
	pkt[0] = ATAPI_CMD_REQUESTSENSE;
	pkt[4] = sizeof buf;

	rc = atapi_packet_read(hd, pkt, 12, (char *)buf, sizeof buf);
	if (rc < 0) {
		return rc;
	}

	return 0;
}

static int cd_read(block_dev_t *bdev, char *buffer,
					size_t count, blkno_t blkno) {
	unsigned char pkt[12];
	unsigned int blks;
	hd_t *hd = (hd_t *) bdev->privdata;

	blks = count / CDSECTORSIZE;
	if (blks > 0xFFFF) {
		return -EINVAL;
	}

	memset(pkt, 0, 12);
	pkt[0] = ATAPI_CMD_READ10;
	pkt[2] = blkno >> 24;
	pkt[3] = (blkno >> 16) & 0xFF;
	pkt[4] = (blkno >> 8) & 0xFF;
	pkt[5] = blkno & 0xFF;
	pkt[7] = (blks >> 8) & 0xFF;
	pkt[8] = blks & 0xFF;

	return atapi_packet_read(hd, pkt, 12, buffer, count);
}

static int cd_write(block_dev_t *bdev, char *buffer,
					size_t count, blkno_t blkno) {
	return -ENODEV;
}

static int cd_ioctl(block_dev_t *bdev, int cmd, void *args, size_t size) {
	hd_t *hd = (hd_t *) bdev->privdata;
	int rc;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		if (hd->blks <= 0) {
			hd->blks = atapi_read_capacity(hd);
		}
		return hd->blks;

	case IOCTL_GETBLKSIZE:
		return CDSECTORSIZE;

	case IOCTL_REVALIDATE:
		rc = atapi_request_sense(hd);
		if (rc < 0) {
			return rc;
		}
		rc = hd->blks = atapi_read_capacity(hd);
		if (rc < 0) {
			return rc;
		}
		return 0;
	}
	return -ENOSYS;
}

static block_dev_driver_t idecd_pio_driver = {
	"idecd_drv",
	cd_ioctl,
	cd_read,
	cd_write
};

static int idecd_init (void *args) {
//	struct ide_tab *ide;
	hd_t *drive;
	size_t size;
//	int i;
	char   path[PATH_MAX];
#if 0
	ide = ide_get_drive();

	for(i = 0; i < HD_DRIVES; i++) {
		if (NULL == ide->drive[i]) {
			continue;
		}

		drive = (hd_t *) ide->drive[i];
#endif
		drive = (hd_t *)args;
		/* Make new device */
		if (drive->media == IDE_CDROM) {
			*path = 0;
			strcat(path, "/dev/cd#");
			if (0 > (drive->idx = block_dev_named(path, &idecd_idx))) {
				return drive->idx;
			}
			drive->bdev = block_dev_create(path, &idecd_pio_driver, drive);

			if (NULL != drive->bdev) {
				if (drive->blks <= 0) {
					drive->blks = atapi_read_capacity(drive);
				}
				size = drive->blks * CDSECTORSIZE;
				block_dev(drive->bdev)->size = size;
			} else {
				return -1;
			}

			drive->blks = 0;
		}
#if 0
	}
#endif
	return 0;
}

EMBOX_BLOCK_DEV("idecd", &idecd_pio_driver, idecd_init);
