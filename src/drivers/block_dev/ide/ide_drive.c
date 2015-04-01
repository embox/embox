/*
 * @file
 * @brief
 *
 * @date 24.08.2012
 * @author Andrey Gazukin
 */


/*
 * hd.c
 *
 * IDE driver
 *
 * Copyright (C) 2002 Michael Ringgaard. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#include <util/indexator.h>

//TODO asm/io.h wrong place
#include <asm/io.h>

#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <drivers/ide.h>
#include <embox/block_dev.h>
#include <mem/phymem.h>

static hdc_t hdctab[HD_CONTROLLERS];
static hd_t  hdtab[HD_DRIVES];
static struct ide_tab ide;
static long tmr_cmd_start_time;

/* general indexator for all ide disk */
INDEX_DEF(harddisk_idx,0,HD_DRIVES);
struct indexator *idedisk_idx;

static void hd_fixstring(unsigned char *s, int len) {
	unsigned char *p = s;
	unsigned char *end = s + len;

	/* Convert from big-endian to host byte order */
	for (p = end ; p != s;) {
		 unsigned short *pp = (unsigned short *) (p -= 2);
		*pp = ((*pp & 0x00FF) << 8) | ((*pp & 0xFF00) >> 8);
	}

	/* Strip leading blanks */
	while (s != end && *s == ' ') {
		++s;
	}

	/* Compress internal blanks and strip trailing blanks */
	while (s != end && *s) {
		if (*s++ != ' ' || (s != end && *s && *s != ' ')) {
			*p++ = *(s - 1);
		}
	}

	/* Wipe out trailing garbage */
	while (p != end) {
		*p++ = '\0';
	}
}

static void hd_error(char *func, unsigned char error) {

#ifdef DEBUG
	printk("%s: ", func);
	if (error & HDCE_BBK)   printk("bad block  ");
	if (error & HDCE_UNC)   printk("uncorrectable data  ");
	if (error & HDCE_MC)    printk("media change  ");
	if (error & HDCE_IDNF)  printk("id not found  ");
	if (error & HDCE_MCR)   printk("media change requested  ");
	if (error & HDCE_ABRT)  printk("abort  ");
	if (error & HDCE_TK0NF) printk("track 0 not found  ");
	if (error & HDCE_AMNF)  printk("address mark not found  ");
	printk("\n");
#endif /*def DEBUG*/

	return;
}

static long system_read_timer(void) {
	struct timespec ts;

	ktime_get_timespec (&ts);
	return (long) ts.tv_sec;
}

static void tmr_set_timeout(void) {
	/* get the command start time */
	tmr_cmd_start_time = system_read_timer();
}

static int tmr_chk_timeout(long timeout) {
	long curTime;

	/* get current time */
	curTime = system_read_timer();

	/* timed out yet ? */
	if (curTime >= (tmr_cmd_start_time + timeout)) {
	  return 1;      /* yes */
	}
	/* no timeout yet */
	return 0;
}


int ide_wait(hdc_t *hdc, unsigned char mask, unsigned int timeout) {
	unsigned char status, error;

	tmr_set_timeout();
	while (1) {
		status = inb(hdc->iobase + HDC_ALT_STATUS);
		if (status & HDCS_ERR) {
			error = inb(hdc->iobase + HDC_ERR);
			hd_error("hdwait", error);

			return -EIO;
		}

		if (!(status & HDCS_BSY) && ((status & mask) == mask)) {
			return 0;
		}
		if (tmr_chk_timeout(timeout)) {
			return -ETIMEDOUT;
		}
		if ((HDCS_DRQ == mask) && (0 == status)) {
			return -EIO;
		}
	}
	return -ETIMEDOUT;
}

void ide_select_drive(hd_t *hd) {
	outb(hd->drvsel, hd->hdc->iobase + HDC_DRVHD);
}

void hd_setup_transfer(hd_t *hd, blkno_t blkno, int nsects) {
	unsigned int track;
	unsigned int head;
	unsigned int sector;

	if (hd->lba) {
		track = (blkno >> 8) & 0xFFFF;
		head = ((blkno >> 24) & 0xF) | HD_LBA;
		sector = blkno & 0xFF;
	} else {
		track = blkno / (hd->heads * hd->sectors);
		head = (blkno / hd->sectors) % hd->heads;
		sector = blkno % hd->sectors + 1;
	}

	outb(0, hd->hdc->iobase + HDC_FEATURE);
	outb(nsects, hd->hdc->iobase + HDC_SECTORCNT);
	outb((unsigned char) sector, hd->hdc->iobase + HDC_SECTOR);
	outb((unsigned char) track, hd->hdc->iobase + HDC_TRACKLSB);
	outb((unsigned char) (track >> 8), hd->hdc->iobase + HDC_TRACKMSB);
	outb((((unsigned char) head & 0xFF) | (unsigned char) hd->drvsel),
		 hd->hdc->iobase + HDC_DRVHD);
}

void pio_read_buffer(hd_t *hd, char *buffer, int size) {
	hdc_t *hdc = hd->hdc;

	if (hd->use32bits) {
		insl(hdc->iobase + HDC_DATA, buffer, size / 4);
	} else {
		insw(hdc->iobase + HDC_DATA, buffer, size / 2);
	}
}

void pio_write_buffer(hd_t *hd, char *buffer, int size) {
	hdc_t *hdc = hd->hdc;

	if (hd->use32bits) {
		outsl(hdc->iobase + HDC_DATA, buffer, size / 4);
	} else {
		outsw(hdc->iobase + HDC_DATA, buffer, size / 2);
	}
}


static int hd_identify(hd_t *hd) {

	/* Ignore interrupt for identify command */
	hd->hdc->dir = HD_XFER_IGNORE;

	/* Issue read drive parameters command */
	outb(0, hd->hdc->iobase + HDC_FEATURE);
	outb(hd->drvsel, hd->hdc->iobase + HDC_DRVHD);
	outb(hd->iftype == HDIF_ATAPI ? HDCMD_PIDENTIFY : HDCMD_IDENTIFY,
			hd->hdc->iobase + HDC_COMMAND);
	/*
	* Some controllers issues the interrupt before data is ready to be read
	* Make sure data is ready by waiting for DRQ to be set
	*/
	if (ide_wait(hd->hdc, HDCS_DRQ, HDTIMEOUT_DRQ) < 0) {
		return -EIO;
	}

	/* Read parameter data */
	insw(hd->hdc->iobase + HDC_DATA,
			(char *) &(hd->param), SECTOR_SIZE / 2);

	/* XXX this was added when ide drive with reported block size equals 64
 	 * However, block dev tries to use this and fails */
	static_assert(SECTOR_SIZE == 512);
	if (hd->param.unfbytes < SECTOR_SIZE) {
		hd->param.unfbytes = SECTOR_SIZE;
	}

	/* Fill in drive parameters */
	hd->cyls = hd->param.cylinders;
	hd->heads = hd->param.heads;
	hd->sectors = hd->param.sectors;
	hd->use32bits = 0;/*hd->param.usedmovsd != 0; */
	hd->sectbufs = hd->param.buffersize;
	hd->multsect = hd->param.nsecperint;
	if (hd->multsect == 0) {
		hd->multsect = 1;
	}

	hd_fixstring((unsigned char *)hd->param.model, sizeof(hd->param.model));
	hd_fixstring((unsigned char *)hd->param.rev, sizeof(hd->param.rev));
	hd_fixstring((unsigned char *)hd->param.serial, sizeof(hd->param.serial));

	if (hd->iftype == HDIF_ATA) {
		hd->media = IDE_DISK;
	} else {
		hd->media = (hd->param.config >> 8) & 0x1f;
	}

	/* Determine LBA or CHS mode */
	if ((hd->param.caps & 0x0200) == 0) {
		hd->lba = 0;
		hd->blks = hd->cyls * hd->heads * hd->sectors;
		if (hd->cyls == 0 && hd->heads == 0 && hd->sectors == 0) {
			return -EIO;
		}
		if (hd->cyls == 0xFFFF && hd->heads == 0xFFFF
			&& hd->sectors == 0xFFFF) {
			return -EIO;
		}
	} else {
		hd->lba = 1;
		hd->blks = (hd->param.totalsec1 << 16) | hd->param.totalsec0;
		if (hd->media == IDE_DISK && (hd->blks == 0 ||
			hd->blks == 0xFFFFFFFF)) {
			return -EIO;
		}
	}

	return 0;
}

static int hd_cmd(hd_t *hd, unsigned int cmd,
				  unsigned int feat, unsigned int nsects) {
	/* Ignore interrupt for command */
	hd->hdc->dir = HD_XFER_IGNORE;

	/* Issue command */
	outb(feat, hd->hdc->iobase + HDC_FEATURE);
	outb(nsects, hd->hdc->iobase + HDC_SECTORCNT);
	outb(hd->drvsel, hd->hdc->iobase + HDC_DRVHD);
	outb(cmd, hd->hdc->iobase + HDC_COMMAND);

	/* Check status */
	if (hd->hdc->result < 0) {
		return -EIO;
	}

	return 0;
}

int hd_ioctl(block_dev_t *bdev, int cmd, void *args, size_t size) {
	struct dev_geometry *geom;
	hd_t *hd = (hd_t *) bdev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return hd->blks;

	case IOCTL_GETBLKSIZE:
		return hd->param.unfbytes;

	case IOCTL_GETGEOMETRY:
		if (!args || size != sizeof(struct dev_geometry)) {
			return -EINVAL;
		}
		geom = (struct dev_geometry *) args;
		geom->cyls = hd->cyls;
		geom->heads = hd->heads;
		geom->spt = hd->sectors;
		geom->sectorsize = hd->param.unfbytes;
		geom->sectors = hd->blks;
		return 0;

	case IOCTL_REVALIDATE:
		return create_partitions(hd);
	}

	return -ENOSYS;
}

static void hd_read_hndl(hdc_t *hdc) {
	unsigned char error;
	int nsects;
	int n;

	/* Check status */
	hdc->status = inb(hdc->iobase + HDC_STATUS);
	if (hdc->status & HDCS_ERR) {
		error = inb(hdc->iobase + HDC_ERR);
		hd_error("hdread", error);
		hdc->result = -EIO;
	} else {
	/* Read sector data */
		nsects = hdc->active->multsect;
		if (nsects > hdc->nsects) {
			nsects = hdc->nsects;
		}
		for (n = 0; n < nsects; n++) {
			pio_read_buffer(hdc->active, hdc->bufp, SECTOR_SIZE);
			hdc->bufp += SECTOR_SIZE;
		}
		hdc->nsects -= nsects;
	}
}

static void hd_write_hndl(hdc_t *hdc) {
	unsigned char error;
	int nsects;
	int n;

	/* Check status */
	hdc->status = inb(hdc->iobase + HDC_STATUS);
	if (hdc->status & HDCS_ERR) {
		error = inb(hdc->iobase + HDC_ERR);
		hd_error("hdwrite", error);

		hdc->result = -EIO;
	} else {
		/* Transfer next sector(s) or signal end of transfer */
		nsects = hdc->active->multsect;
		if (nsects > hdc->nsects) {
			nsects = hdc->nsects;
		}
		hdc->nsects -= nsects;

		if (hdc->nsects > 0) {
			nsects = hdc->active->multsect;
			if (nsects > hdc->nsects) {
				nsects = hdc->nsects;
			}

			for (n = 0; n < nsects; n++) {
				pio_write_buffer(hdc->active, hdc->bufp, SECTOR_SIZE);
				hdc->bufp += SECTOR_SIZE;
			}
		}
	}
}

static irq_return_t hdc_handler(unsigned int irq_num, void *arg) {
	hdc_t *hdc = (hdc_t *) arg;

	switch (hdc->dir) {
	case HD_XFER_READ:
		hd_read_hndl(hdc);
		break;

	case HD_XFER_WRITE:
		hd_write_hndl(hdc);
		break;

	case HD_XFER_DMA:
		outb(inb(hdc->bmregbase + BM_STATUS_REG),
				hdc->bmregbase + BM_STATUS_REG);
		hdc->status = inb(hdc->iobase + HDC_STATUS);
		break;

	case HD_XFER_IGNORE:
		/* Read status to acknowledge interrupt */
		hdc->status = inb(hdc->iobase + HDC_STATUS);
		break;

	case HD_XFER_IDLE:
	default:
		/* Read status to acknowledge interrupt */
		hdc->status = inb(hdc->iobase + HDC_STATUS);
		break;
	}

	if ((0 == hdc->result) && (HD_XFER_IDLE != hdc->dir)
			              && (HD_XFER_IGNORE != hdc->dir)) {
		hdc->result = 1;
		waitq_wakeup_all(&hdc->waitq);
	}

	return IRQ_HANDLED;
}

static int probe_device(hdc_t *hdc, int drvsel) {
	unsigned char sc, sn;

	/* Probe for device on controller */
	outb(drvsel, hdc->iobase + HDC_DRVHD);
	idedelay();

	outb(0x55, hdc->iobase + HDC_SECTORCNT);
	outb(0xAA, hdc->iobase + HDC_SECTOR);

	outb(0xAA, hdc->iobase + HDC_SECTORCNT);
	outb(0x55, hdc->iobase + HDC_SECTOR);

	outb(0x55, hdc->iobase + HDC_SECTORCNT);
	outb(0xAA, hdc->iobase + HDC_SECTOR);

	sc = inb(hdc->iobase + HDC_SECTORCNT);
	sn = inb(hdc->iobase + HDC_SECTOR);

	if (sc == 0x55 && sn == 0xAA) {
		return 1;
	} else {
		return -EIO;
	}
}

static int wait_reset_done(hdc_t *hdc, int drvsel) {

	outb(drvsel, hdc->iobase + HDC_DRVHD);
	idedelay();

	tmr_set_timeout();
	while (1) {
		hdc->status = inb(hdc->iobase + HDC_STATUS);
		if ((hdc->status & HDCS_BSY) == 0) {
			return 0;
		}
		if (tmr_chk_timeout(HDTIMEOUT_DRDY)) {
			break;
		}
	}

	return -EBUSY;
}

static int get_interface_type(hdc_t *hdc, int drvsel) {
	unsigned char sc, sn, cl, ch, st;

	outb(drvsel, hdc->iobase + HDC_DRVHD);
	idedelay();

	sc = inb(hdc->iobase + HDC_SECTORCNT);
	sn = inb(hdc->iobase + HDC_SECTOR);

	if (sc == 0x01 && sn == 0x01) {
		cl = inb(hdc->iobase + HDC_TRACKLSB);
		ch = inb(hdc->iobase + HDC_TRACKMSB);
		st = inb(hdc->iobase + HDC_STATUS);

		if (cl == 0x14 && ch == 0xeb) {
			return HDIF_ATAPI;
		}
		if (cl == 0x00 && ch == 0x00 && st != 0x00) {
			return HDIF_ATA;
		}
	}

	return HDIF_UNKNOWN;
}

static int setup_controller(hdc_t *hdc, int iobase, int irq,
					int bmregbase, int *masterif, int *slaveif) {
	int res;

	memset(hdc, 0, sizeof(hdc_t));
	hdc->iobase = iobase;
	hdc->irq = irq;
	hdc->bmregbase = bmregbase;
	hdc->dir = HD_XFER_IGNORE;
	waitq_init(&hdc->waitq);

	if (hdc->bmregbase) {
		if (hdc->prds) {
			phymem_free(hdc->prds, 1);
		}
		/* Allocate one page for PRD list */
		hdc->prds = (struct prd *) phymem_alloc(1);
	}

	/* Assume no devices connected to controller */
	*masterif = HDIF_NONE;
	*slaveif = HDIF_NONE;

	/* Setup device control register */
	outb(HDDC_HD15 | HDDC_NIEN, hdc->iobase + HDC_CONTROL);

	/* Probe for master and slave device on controller */
	if (probe_device(hdc, HD0_DRVSEL) >= 0) {
		*masterif = HDIF_PRESENT;
	}
	if (probe_device(hdc, HD1_DRVSEL) >= 0) {
		*slaveif = HDIF_PRESENT;
	}

	/* Reset controller */
	outb(HDDC_HD15 | HDDC_SRST | HDDC_NIEN, hdc->iobase + HDC_CONTROL);
	idedelay();
	outb(HDDC_HD15 | HDDC_NIEN, hdc->iobase + HDC_CONTROL);
	idedelay();

	/* Wait for reset to finish on all present devices */
	if (*masterif != HDIF_NONE) {
		int rc = wait_reset_done(hdc, HD0_DRVSEL);
		if (rc < 0) {
			*masterif = HDIF_NONE;
		}
	}

	if (*slaveif != HDIF_NONE) {
		int rc = wait_reset_done(hdc, HD1_DRVSEL);
		if (rc < 0) {
			*slaveif = HDIF_NONE;
		}
	}

	/* Determine interface types */
	if (*masterif != HDIF_NONE) {
		*masterif = get_interface_type(hdc, HD0_DRVSEL);
	}
	if (*slaveif != HDIF_NONE) {
		*slaveif = get_interface_type(hdc, HD1_DRVSEL);
	}

	/* Enable interrupts */
	res = irq_attach(hdc->irq, hdc_handler, 0, hdc, "ide");
	if (res < 0) {
		return res;
	}

	outb(HDDC_HD15, hdc->iobase + HDC_CONTROL);
	idedelay();

	return 0;
}

static int ide_create_block_dev(hd_t *hd) {
	const block_dev_module_t *bdev;

	switch (hd->media) {
		case IDE_CDROM:
			bdev = block_dev_lookup("idecd");

			break;


		case IDE_DISK:	{
			if (hd->udmamode == -1) {
				bdev = block_dev_lookup("idedisk");
			} else {
				bdev = block_dev_lookup("idedisk_udma");
			}

			break;
		}
		default: {
			bdev = NULL;
			return 0;
		}
	}
	if (bdev == NULL) {
		return 0;
	}
	bdev->init(hd);

	return 0;
}
static void setup_hd(hd_t *hd, hdc_t *hdc, int drvsel,
					 int udmasel, int iftype, int numslot) {
	/* static int udma_speed[] = {16, 25, 33, 44, 66, 100}; */

	int rc;

	/* Initialize drive block */
	memset(hd, 0, sizeof(hd_t));
	hd->hdc = hdc;
	hd->drvsel = drvsel;
	hd->iftype = iftype;

	/* Get info block from device */
	rc = hd_identify(hd);
	if (rc < 0) {
		/* Try other interface type */
		if (hd->iftype == HDIF_ATA) {
			hd->iftype = HDIF_ATAPI;
		} else if (hd->iftype == HDIF_ATAPI) {
			hd->iftype = HDIF_ATA;
		}
		rc = hd_identify(hd);
		if (rc < 0) {
			ide.drive[numslot] = NULL;
			return;
		}
	}
	ide.drive[numslot]  = (hd_t *)hd;

	/* Determine UDMA mode */
	if (!hdc->bmregbase) {
		hd->udmamode = -1;
	} else if ((hd->param.valid & 4) &&
			(hd->param.dmaultra & (hd->param.dmaultra >> 8) & 0x3F)) {
		if ((hd->param.dmaultra >> 13) & 1) {
			hd->udmamode = 5; /* UDMA 100 */
		} else if ((hd->param.dmaultra >> 12) & 1) {
			hd->udmamode = 4; /* UDMA 66 */
		} else if ((hd->param.dmaultra >> 11) & 1) {
			hd->udmamode = 3; /* UDMA 44 */
		} else if ((hd->param.dmaultra >> 10) & 1) {
			hd->udmamode = 2; /* UDMA 33 */
		} else if ((hd->param.dmaultra >> 9) & 1) {
			hd->udmamode = 1; /* UDMA 25 */
		} else {
			hd->udmamode = 0; /* UDMA 16 */
		}
	} else {
		hd->udmamode = -1;
	}

	/* Set multi-sector mode if drive supports it */
	if (hd->multsect > 1) {
		rc = hd_cmd(hd, HDCMD_SETMULT, 0, hd->multsect);
		if (rc < 0) {
			hd->multsect = 1;
		}
	}

	/* Enable UDMA for drive if it supports it. */
	if (hd->udmamode != -1) {
		/* Enable drive in bus master status register */
		int dmastat = inb(hdc->bmregbase + BM_STATUS_REG);
		outb(dmastat | udmasel, hdc->bmregbase + BM_STATUS_REG);

		/* Set feature in IDE controller */
		rc = hd_cmd(hd, HDCMD_SETFEATURES, HDFEAT_XFER_MODE,
				HDXFER_MODE_UDMA | hd->udmamode);
	}

	/* Enable read ahead and write caching if supported */
	if (hd->param.csfo & 2) {
		hd_cmd(hd, HDCMD_SETFEATURES, HDFEAT_ENABLE_RLA, 0);
	}
	if (hd->param.csfo & 1) {
		hd_cmd(hd, HDCMD_SETFEATURES, HDFEAT_ENABLE_WCACHE, 0);
	}

	ide_create_block_dev(hd);
}

static int ide_init(void) {
	int rc;
	int masterif;
	int slaveif;
	int numhd;
	numhd = 4;

	idedisk_idx = &harddisk_idx;

	if (numhd >= 1)  {
		/*
		rc = setup_controller(&hdctab[0], HDC0_IOBASE, HDC0_IRQ,
						ide ? bmiba : 0, &masterif, &slaveif);
		*/
		rc = setup_controller(&hdctab[0], HDC0_IOBASE,
				HDC0_IRQ, 0, &masterif, &slaveif);
		if (rc >= 0) {
			if (numhd >= 1 && masterif > HDIF_UNKNOWN) {
				setup_hd(&hdtab[0], &hdctab[0], HD0_DRVSEL,
						BM_SR_DRV0, masterif, 0);
			}
			if (numhd >= 2 && slaveif > HDIF_UNKNOWN) {
				setup_hd(&hdtab[1], &hdctab[0], HD1_DRVSEL,
						BM_SR_DRV1, slaveif, 1);
			}
		}
	}

	if (numhd >= 3) {
		/*
		rc = setup_controller(&hdctab[1], HDC1_IOBASE,
						HDC1_IRQ, ide ? bmiba + 8 : 0, &masterif, &slaveif);
		*/
		rc = setup_controller(&hdctab[1], HDC1_IOBASE,
				HDC1_IRQ, 0, &masterif, &slaveif);
		if (rc >= 0) {
			if (numhd >= 3 && masterif > HDIF_UNKNOWN) {
				setup_hd(&hdtab[2], &hdctab[1], HD0_DRVSEL,
						BM_SR_DRV0, masterif, 2);
			}
			if (numhd >= 4 && slaveif > HDIF_UNKNOWN) {
				setup_hd(&hdtab[3], &hdctab[1], HD1_DRVSEL,
						BM_SR_DRV1, slaveif, 3);
			}
		}
	}
	return 0;
}

struct ide_tab *ide_get_drive(void) {
	return &ide;
}

EMBOX_UNIT_INIT(ide_init);
