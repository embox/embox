/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Andrey Gazukin
 */

#include <asm/io.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <net/in.h>
#include <fs/fat.h>
#include <drivers/ide.h>
#include <embox/block_dev.h>
#include <mem/phymem.h>
#include <util/indexator.h>

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
	while (!hdc->result) {
		/* Wait until data ready */
		usleep(300);

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
	if(0 < hdc->result) {
		result = hdc->result = 0;
	}
	return result == 0 ? bufsize - bufleft : result;
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

static int cd_read(block_dev_t *dev, char *buffer,
					size_t count, blkno_t blkno) {
	unsigned char pkt[12];
	unsigned int blks;
	hd_t *hd = (hd_t *) dev->privdata;

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

static int cd_write(block_dev_t *dev, char *buffer,
					size_t count, blkno_t blkno) {
	return -ENODEV;
}

static int cd_ioctl(block_dev_t *dev, int cmd, void *args, size_t size) {
	hd_t *hd = (hd_t *) dev->privdata;
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
	struct ide_tab *ide;
	hd_t *drive;
	dev_t name_idx;
	double size;
	char   path[MAX_LENGTH_PATH_NAME];

	ide = ide_get_drive();

	for(int i = 0; i < HD_DRIVES; i++) {
		if(NULL == ide->drive[i]) {
			continue;
		}
		else {
			drive = (hd_t *) ide->drive[i];
			/* Make new device */
			if (drive->media == IDE_CDROM) {
				*path = 0;
				strcat(path, "/dev/");
				name_idx = (dev_t) index_alloc(&idecd_idx, INDEX_ALLOC_MIN);
				drive->dev_id = block_dev_create(strcat(path, "cd#"),
						&idecd_pio_driver, drive, &name_idx);

				if(NULL != drive->dev_id) {
					size = (double) drive->param.cylinders *
						   (double) drive->param.heads *
						   (double) drive->param.unfbytes *
						   (double) (drive->param.sectors + 1);
					block_dev(drive->dev_id)->size = (size_t) size;
				}
				else {
					return -1;
				}

				drive->blks = 0;
			}
			else {
				continue;
			}
		}
	}
	return 0;
}

EMBOX_BLOCK_DEV("idecd", &idecd_pio_driver, idecd_init);
