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

extern int hd_ioctl(block_dev_t *dev, int cmd, void *args, size_t size);

static void setup_dma(hdc_t *hdc, char *buffer, int count, int cmd) {
	int i;
	int len;
	char *next;

	i = 0;
	next = (char *) ((unsigned long) buffer & ~(PAGESIZE - 1)) + PAGESIZE;
	while (1) {
		hdc->prds[i].addr = (unsigned long) buffer;
		len = next - buffer;
		if (count > len) {
			hdc->prds[i].len = len;
			count -= len;
			buffer = next;
			next += PAGESIZE;
			i++;
		}
		else {
			hdc->prds[i].len = count | 0x80000000;
			break;
		}
	}

	/* Setup PRD table */
	outl(hdc->prds_phys, hdc->bmregbase + BM_PRD_ADDR);

	/* Specify read/write */
	outb(cmd | BM_CR_STOP, hdc->bmregbase + BM_COMMAND_REG);

	/* Clear INTR & ERROR flags */
	outb(inb(hdc->bmregbase + BM_STATUS_REG) | BM_SR_INT | BM_SR_ERR,
		 hdc->bmregbase + BM_STATUS_REG);
}

static void start_dma(hdc_t *hdc) {
	/* Start DMA operation */
	outb(inb(hdc->bmregbase + BM_COMMAND_REG) | BM_CR_START,
		 hdc->bmregbase + BM_COMMAND_REG);
}

static int stop_dma(hdc_t *hdc) {
	int dmastat;

	/* Stop DMA channel and check DMA status */
	outb(inb(hdc->bmregbase + BM_COMMAND_REG) & ~BM_CR_START,
	   hdc->bmregbase + BM_COMMAND_REG);

	/* Get DMA status */
	dmastat = inb(hdc->bmregbase + BM_STATUS_REG);

	/* Clear INTR && ERROR flags */
	outb(dmastat | BM_SR_INT | BM_SR_ERR, hdc->bmregbase + BM_STATUS_REG);

	/* Check for DMA errors */
	if (dmastat & BM_SR_ERR) {
		return -EIO;
	}

	return 0;
}

static int hd_read_udma(block_dev_t *dev, char *buffer, size_t count, blkno_t blkno) {
	hd_t *hd;
	hdc_t *hdc;
	int sectsleft;
	int nsects;
	int result;
	char *bufp;

	if (count == 0) {
		return 0;
	}
	bufp = (char *) buffer;

	hd = (hd_t *) dev->privdata;
	hdc = hd->hdc;
	sectsleft = count / SECTOR_SIZE;


	while (sectsleft > 0) {
		/* Select drive */
		ide_select_drive(hd);

		/* Wait for controller ready */
		result = ide_wait(hdc, HDCS_DRDY, HDTIMEOUT_DRDY);
		if (result != 0) {
			result = -EIO;
			break;
		}

		/* Calculate maximum number of sectors we can transfer */
		if (sectsleft > 256) {
			nsects = 256;
		}
		else {
			nsects = sectsleft;
		}

		if (nsects > MAX_DMA_XFER_SIZE / SECTOR_SIZE) {
			nsects = MAX_DMA_XFER_SIZE / SECTOR_SIZE;
		}

		/* Prepare transfer */
		result = 0;
		hdc->dir = HD_XFER_DMA;
		hdc->active = hd;

		hd_setup_transfer(hd, blkno, nsects);

		/* Setup DMA */
		setup_dma(hdc, bufp, nsects * SECTOR_SIZE, BM_CR_WRITE);

		/* Start read */
		outb(HDCMD_READDMA, hdc->iobase + HDC_COMMAND);
		start_dma(hdc);

		/* Stop DMA channel and check DMA status */
		result = stop_dma(hdc);
		if (result < 0) {
			break;
		}

		/* Check controller status */
		if (hdc->status & HDCS_ERR) {
			result = -EIO;
			break;
		}

		/* Advance to next */
		sectsleft -= nsects;
		bufp += nsects * SECTOR_SIZE;
	}

	/* Cleanup */
	hdc->dir = HD_XFER_IDLE;
	hdc->active = NULL;

	return result == 0 ? count : result;
}

static int hd_write_udma(block_dev_t *dev, char *buffer, size_t count, blkno_t blkno) {
	hd_t *hd;
	hdc_t *hdc;
	int sectsleft;
	int nsects;
	int result;
	char *bufp;

	if (count == 0) {
		return 0;
	}
	bufp = (char *) buffer;

	hd = (hd_t *) dev->privdata;
	hdc = hd->hdc;
	sectsleft = count / SECTOR_SIZE;

	while (sectsleft > 0) {
		/* Select drive */
		ide_select_drive(hd);

		/* Wait for controller ready */
		result = ide_wait(hdc, HDCS_DRDY, HDTIMEOUT_DRDY);
		if (result != 0) {
			result = -EIO;
			break;
		}

		/* Calculate maximum number of sectors we can transfer */
		if (sectsleft > 256) {
			nsects = 256;
		}
		else {
			nsects = sectsleft;
		}
		if (nsects > MAX_DMA_XFER_SIZE / SECTOR_SIZE) {
			nsects = MAX_DMA_XFER_SIZE / SECTOR_SIZE;
		}

		/* Prepare transfer */
		result = 0;
		hdc->dir = HD_XFER_DMA;
		hdc->active = hd;

		hd_setup_transfer(hd, blkno, nsects);

		/* Setup DMA */
		setup_dma(hdc, bufp, nsects * SECTOR_SIZE, BM_CR_READ);

		/* Start write */
		outb(HDCMD_WRITEDMA, hdc->iobase + HDC_COMMAND);
		start_dma(hdc);

		/* Stop DMA channel and check DMA status */
		result = stop_dma(hdc);
		if (result < 0) {
			break;
		}

		/* Check controller status */
		if (hdc->status & HDCS_ERR) {
			result = -EIO;
			break;
		}

		/* Advance to next */
		sectsleft -= nsects;
		bufp += nsects * SECTOR_SIZE;
	}

	/* Cleanup */
	hdc->dir = HD_XFER_IDLE;
	hdc->active = NULL;

	return result == 0 ? count : result;
}

static block_dev_driver_t idedisk_udma_driver = {
	"idedisk_udma_drv",
	hd_ioctl,
	hd_read_udma,
	hd_write_udma
};

static int idedisk_udma_init (void *args) {
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
			if ((drive->media == IDE_DISK) && (drive->udmamode != -1)) {
				*path = 0;
				strcat(path, "/dev/");
				name_idx = (dev_t) index_alloc(idedisk_idx, INDEX_ALLOC_MIN);
				drive->dev_id = block_dev_create(strcat(path, "hd*"),
						&idedisk_udma_driver, drive, &name_idx);

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
				create_partitions(drive);
			}
			else {
				continue;
			}
		}
	}
	return 0;
}

EMBOX_BLOCK_DEV("idedisk_udma", &idedisk_udma_driver, idedisk_udma_init);
