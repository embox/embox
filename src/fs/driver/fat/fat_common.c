/**
 * @file
 * @brief  VFS-independent part of FAT driver
 * @data   9 Apr 2015
 * @author Denis Deryugin
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drivers/block_dev.h>
#include <kernel/printk.h>
#include <fs/fat.h>
#include <mem/misc/pool.h>
#include <util/math.h>

#define FAT_USE_LONG_NAMES OPTION_GET(BOOLEAN, fat_max_sector_size)

#define LABEL    "EMBOX_DISK " /* Whitespace-padded 11-char string */
#define SYSTEM12 "FAT12   "
#define SYSTEM16 "FAT16   "
#define SYSTEM32 "FAT32   "

uint8_t fat_sector_buff[FAT_MAX_SECTOR_SIZE] __attribute__((aligned(16)));

size_t bdev_blk_sz(struct block_dev *bdev) {
	return bdev->block_size;
}

static const char bootcode[130] =
	{ 0x0e, 0x1f, 0xbe, 0x5b, 0x7c, 0xac, 0x22, 0xc0, 0x74, 0x0b,
	  0x56, 0xb4, 0x0e, 0xbb, 0x07, 0x00, 0xcd, 0x10, 0x5e, 0xeb,
	  0xf0, 0x32, 0xe4, 0xcd, 0x16, 0xcd, 0x19, 0xeb, 0xfe, 0x54,
	  0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74,
	  0x20, 0x61, 0x20, 0x62, 0x6f, 0x6f, 0x74, 0x61, 0x62, 0x6c,
	  0x65, 0x20, 0x64, 0x69, 0x73, 0x6b, 0x2e, 0x20, 0x20, 0x50,
	  0x6c, 0x65, 0x61, 0x73, 0x65, 0x20, 0x69, 0x6e, 0x73, 0x65,
	  0x72, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6f, 0x6f, 0x74, 0x61,
	  0x62, 0x6c, 0x65, 0x20, 0x66, 0x6c, 0x6f, 0x70, 0x70, 0x79,
	  0x20, 0x61, 0x6e, 0x64, 0x0d, 0x0a, 0x70, 0x72, 0x65, 0x73,
	  0x73, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x20,
	  0x74, 0x6f, 0x20, 0x74, 0x72, 0x79, 0x20, 0x61, 0x67, 0x61,
	  0x69, 0x6e, 0x20, 0x2e, 0x2e, 0x2e, 0x20, 0x0d, 0x0a, 0x00 };

static int fat_sec_by_clus(struct fat_fs_info *fsi, int clus) {
	return clus > 2 ? (clus - 2) * fsi->vi.secperclus + fsi->vi.dataarea : 0;
}

extern int fat_read_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector);
extern int fat_write_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector);

/**
 * @brief Format given block device
 *
 * @param dev Block device to be formatted
 * @param fat_n should be 12/16/32 or 0
 *
 * @return Negative error code or 0 if succeed
 */
int fat_create_partition(void *dev, int fat_n) {
	struct block_dev *bdev = dev;
	uint16_t bytepersec = bdev->block_size;
	size_t num_sect = block_dev(bdev)->size / bytepersec;
	assert(bdev->block_size <= FAT_MAX_SECTOR_SIZE);
	uint32_t secperfat = 1;
	uint16_t rootentries = 0x0200;             /* 512 for FAT16 */
	int reserved;
	int err;
	int i;
	int fat_size;
	int blkpersec;

	struct lbr lbr = (struct lbr) {
		.jump = {0xeb, 0x3c, 0x90},        /* JMP 0x3c; NOP; */
		.oemid = {0x45, 0x45, 0x45, 0x45,
		          0x45, 0x45, 0x45, 0x45}, /* ASCII "EEEEEEEE" */
		.bpb = {
			.bytepersec_l  = (uint8_t)(bytepersec & 0xFF),
			.bytepersec_h  = (uint8_t)((bytepersec & 0xFF00) >> 8),
			.secperclus    = 0x04,
			.reserved_l    = 0x01,     /* reserved sectors */
			.reserved_h    = 0x00,
			.numfats       = 0x02,     /* 2 FAT copy */
			.rootentries_l = (uint8_t)(0x00FF & rootentries ),
			.rootentries_h = (uint8_t)(0x00FF & (rootentries >> 8)),
			.mediatype     = 0xF8,     /* Fixed disk */
			.secpertrk_l   = 0x3F,     /* TODO use actual value? */
			.heads_l       = 0xFF,     /* TODO use actual value? */
			/* TODO handle hidden sectors? */
		},
		.sig_55 = 0x55,
		.sig_aa = 0xAA,
	};


	if (0xFFFF > num_sect)	{
		lbr.bpb.sectors_s_l = (uint8_t)(0x00000FF & num_sect);
		lbr.bpb.sectors_s_h = (uint8_t)(0x00000FF & (num_sect >> 8));
	} else {
		lbr.bpb.sectors_l_0 = (uint8_t)(0x00000FF & num_sect );
		lbr.bpb.sectors_l_1 = (uint8_t)(0x00000FF & (num_sect >> 8));
		lbr.bpb.sectors_l_2 = (uint8_t)(0x00000FF & (num_sect >> 16));
		lbr.bpb.sectors_l_3 = (uint8_t)(0x00000FF & (num_sect >> 24));
	}

	switch (fat_n) {
	case 12:
	case 16:
		lbr.bpb.secperfat_l   = (uint8_t)(0x00FF & secperfat),
		lbr.bpb.secperfat_h   = (uint8_t)(0x00FF & (secperfat >> 8)),
		lbr.ebpb.ebpb = (struct ebpb) {
			.unit      = 0x80, /* Hard disk */
			.signature = 0x29,
			.serial_0  = 0x81, /* We can actually ignore .serial_N */
			.serial_1  = 0xDB,
			.serial_2  = 0xF7,
			.serial_3  = 0xBB,
		};

		memcpy(lbr.ebpb.ebpb.label, LABEL, sizeof(lbr.ebpb.ebpb.label));
		memcpy(lbr.ebpb.ebpb.system,
		       fat_n == 12 ? SYSTEM12:SYSTEM16,
		       sizeof(lbr.ebpb.ebpb.system));

		memcpy(lbr.ebpb.ebpb.code, bootcode, sizeof(bootcode));
		break;
	case 32:
		lbr.ebpb.ebpb32 = (struct ebpb32) {
			.fatsize_0 = (uint8_t) (0xFF & secperfat),
			.fatsize_1 = (uint8_t) (0xFF & (secperfat >> 8)),
			.fatsize_2 = (uint8_t) (0xFF & (secperfat >> 16)),
			.fatsize_3 = (uint8_t) (0xFF & (secperfat >> 24)),
			/* TODO fill flags? */
			.fsver_l = 0x1, /* TODO fill actual fat version */
			.fsver_h = 0x1,
			.root_0  = 0x2, /* Root directory is in the second cluster */
			/* TODO fill fsinfo */
			.unit      = 0x80, /* Hard disk */
			.signature = 0x29,
			.serial_0  = 0x81, /* We can actually ignore .serial_N */
			.serial_1  = 0xDB,
			.serial_2  = 0xF7,
			.serial_3  = 0xBB,
			.label     = LABEL,
			.system    = SYSTEM32,
		};
		memcpy(lbr.ebpb.ebpb32.label, LABEL, sizeof(lbr.ebpb.ebpb32.label));
		memcpy(lbr.ebpb.ebpb32.system, SYSTEM32, sizeof(lbr.ebpb.ebpb32.system));
		memcpy(lbr.ebpb.ebpb32.code, bootcode, sizeof(bootcode));
		break;
	default:
		return -1;
	}

	if (0 > (err = block_dev_write(bdev, (void *) &lbr, sizeof(lbr), 0)))
		return err;

	/* Clear FAT from garbage */
	memset(fat_sector_buff, 0, sizeof(fat_sector_buff));
	reserved = (uint16_t) lbr.bpb.reserved_l |
		(((uint16_t) lbr.bpb.reserved_h) << 8);
	fat_size = lbr.bpb.numfats * secperfat;
	blkpersec = bytepersec / bdev->block_size;

	for (i = reserved; i < reserved + fat_size; i++)
		if (0 > (err = block_dev_write(bdev, (void*) fat_sector_buff, sizeof(fat_sector_buff), i * blkpersec)))
			return err;

	return 0;
}

/**
 *	Get starting sector# of specified partition on drive #unit
 *	NOTE: This code ASSUMES an MBR on the disk.
 *	p_scratchsector should point to a SECTOR_SIZE scratch area
 *	Returns 0xffffffff for any error.
 *	If pactive is non-NULL, this function also returns partition active flag.
 *	If pptype is non-NULL, this function also returns the partition type.
 *	If psize is non-NULL, this function also returns the partition size.
 */
uint32_t fat_get_ptn_start(void *bdev, uint8_t pnum, uint8_t *pactive,
		uint8_t *pptype, uint32_t *psize) {
	uint32_t result;
	struct mbr *mbr = (struct mbr*) fat_sector_buff;

	/* DOS ptable supports maximum 4 partitions */
	if (pnum > 3) {
		return DFS_ERRMISC;
	}

	if (0 > block_dev_read(bdev, (char *) fat_sector_buff, sizeof(struct mbr), 0)) {
		return DFS_ERRMISC;
	}
	/* check if that a lbr */
	if ((mbr->bootcode[0] == 0xeb) &&
		(mbr->bootcode[1] == 0x3c) &&
		(mbr->bootcode[2] == 0x90)) {
		return 0;
	}

	result = (uint32_t) mbr->ptable[pnum].start_0 |
		(((uint32_t) mbr->ptable[pnum].start_1) << 8) |
		(((uint32_t) mbr->ptable[pnum].start_2) << 16) |
		(((uint32_t) mbr->ptable[pnum].start_3) << 24);

	if (pactive) {
		*pactive = mbr->ptable[pnum].active;
	}

	if (pptype) {
		*pptype = mbr->ptable[pnum].type;
	}

	if (psize) {
		*psize = (uint32_t) mbr->ptable[pnum].size_0 |
			(((uint32_t) mbr->ptable[pnum].size_1) << 8) |
			(((uint32_t) mbr->ptable[pnum].size_2) << 16) |
			(((uint32_t) mbr->ptable[pnum].size_3) << 24);
	}

	return result;
}

/**
 * Retrieve volume info from BPB and store it in a volinfo_t structure
 * You must provide the unit and starting sector of the filesystem, and
 * a pointer to a sector buffer for scratch
 * Attempts to read BPB and glean information about the FS from that.
 * Returns 0 OK, nonzero for any error.
 */
uint32_t fat_get_volinfo(void *bdev, struct volinfo * volinfo, uint32_t startsector) {
	struct lbr *lbr = (struct lbr *) fat_sector_buff;

	if (0 > block_dev_read(	bdev,
				(char *) fat_sector_buff,
				sizeof(struct lbr),
				0)) { /* TODO start sector */
		return DFS_ERRMISC;
	}

	volinfo->bytepersec = lbr->bpb.bytepersec_l + (lbr->bpb.bytepersec_h << 8);
	volinfo->startsector = startsector;
	volinfo->secperclus = lbr->bpb.secperclus;
	volinfo->reservedsecs = (uint16_t) lbr->bpb.reserved_l |
		  (((uint16_t) lbr->bpb.reserved_h) << 8);

	volinfo->numsecs =  (uint16_t) lbr->bpb.sectors_s_l |
		  (((uint16_t) lbr->bpb.sectors_s_h) << 8);

	if (!volinfo->numsecs)
		volinfo->numsecs = (uint32_t) lbr->bpb.sectors_l_0 |
		  (((uint32_t) lbr->bpb.sectors_l_1) << 8) |
		  (((uint32_t) lbr->bpb.sectors_l_2) << 16) |
		  (((uint32_t) lbr->bpb.sectors_l_3) << 24);

	/**
	 * If secperfat is 0, we must be in a FAT32 volume
	 */
	volinfo->secperfat =  (uint16_t) lbr->bpb.secperfat_l |
		  (((uint16_t) lbr->bpb.secperfat_h) << 8);

	if (!volinfo->secperfat) {
		volinfo->secperfat = (uint32_t) lbr->ebpb.ebpb32.fatsize_0 |
		  (((uint32_t) lbr->ebpb.ebpb32.fatsize_1) << 8) |
		  (((uint32_t) lbr->ebpb.ebpb32.fatsize_2) << 16) |
		  (((uint32_t) lbr->ebpb.ebpb32.fatsize_3) << 24);

		memcpy(volinfo->label, lbr->ebpb.ebpb32.label, MSDOS_NAME);
		volinfo->label[11] = 0;
	} else {
		memcpy(volinfo->label, lbr->ebpb.ebpb.label, MSDOS_NAME);
		volinfo->label[11] = 0;
	}

	/* note: if rootentries is 0, we must be in a FAT32 volume. */
	volinfo->rootentries =  (uint16_t) lbr->bpb.rootentries_l |
		  (((uint16_t) lbr->bpb.rootentries_h) << 8);

	volinfo->fat1 = startsector + volinfo->reservedsecs;

	/**
	 * The calculation below is designed to round up the root directory size
	 * for FAT12/16 and to simply ignore the root directory for FAT32, since
	 * it's a normal, expandable file in that situation.
	 */

	if (volinfo->rootentries) {
		volinfo->rootdir = volinfo->fat1 + (volinfo->secperfat * 2);
		volinfo->dataarea = volinfo->rootdir
			+ (((volinfo->rootentries * 32) + (volinfo->bytepersec - 1))
			/ volinfo->bytepersec);
	} else {
		volinfo->dataarea = volinfo->fat1 + (volinfo->secperfat * 2);
		volinfo->rootdir = (uint32_t) lbr->ebpb.ebpb32.root_0 |
		  (((uint32_t) lbr->ebpb.ebpb32.root_1) << 8) |
		  (((uint32_t) lbr->ebpb.ebpb32.root_2) << 16) |
		  (((uint32_t) lbr->ebpb.ebpb32.root_3) << 24);
	}

	if (0 == volinfo->secperclus) {
		volinfo->numclusters = 0;
		return DFS_ERRMISC;
	} else {
		volinfo->numclusters = (volinfo->numsecs - volinfo->dataarea) /
			volinfo->secperclus;
	}

	if (volinfo->numclusters < 4085)
		volinfo->filesystem = FAT12;
	else if (volinfo->numclusters < 65525)
		volinfo->filesystem = FAT16;
	else
		volinfo->filesystem = FAT32;

	return DFS_OK;
}

/*
 *	Fetch FAT entry for specified cluster number You must provide a scratch
 *	buffer for one sector (SECTOR_SIZE) and a populated struct volinfo
 *	Returns a FAT32 BAD_CLUSTER value for any error, otherwise the contents
 *	of the desired FAT entry.
 *	p_scratchcache should point to a UINT32. This variable caches the physical
 *	sector number last read into the scratch buffer for performance
 *	enhancement reasons.
 */
uint32_t fat_get_fat_(struct fat_fs_info *fsi,
		uint8_t *p_scratch,	uint32_t *p_scratchcache, uint32_t cluster) {
	uint32_t offset, sector, result;
	struct volinfo *volinfo = &fsi->vi;

	switch (volinfo->filesystem) {
		case FAT12:
			offset = cluster + (cluster / 2);
			break;
		case FAT16:
			offset = cluster * 2;
			break;
		case FAT32:
			offset = cluster * 4;
			break;
		default:
			return DFS_BAD_CLUS;
	}

	sector = offset / volinfo->bytepersec + volinfo->fat1;

	if (sector != *p_scratchcache) {
		if (fat_read_sector(fsi, p_scratch, sector)) {
			/*
			 * avoid anyone assuming that this cache value is still valid,
			 * which might cause disk corruption
			 */
			*p_scratchcache = 0;
			return DFS_BAD_CLUS;
		}
		*p_scratchcache = sector;
	}

	/*
	 * At this point, we "merely" need to extract the relevant entry.
	 * This is easy for FAT16 and FAT32, but a royal PITA for FAT12 as
	 * a single entry may span a sector boundary. The normal way around this is
	 * always to read two FAT sectors, but that luxury is (by design intent)
	 * unavailable to DOSFS.
	 */
	offset %= volinfo->bytepersec;
	if (volinfo->filesystem == FAT12) {
		/* Special case for sector boundary - Store last byte of current sector
		 * Then read in the next sector and put the first byte of that sector
		 * into the high byte of result.
		 */
		if (offset == volinfo->bytepersec - 1) {
			result = (uint32_t) p_scratch[offset];
			sector++;
			if (fat_read_sector(fsi, p_scratch, sector)) {
				/*
				 * avoid anyone assuming that this cache value is still valid,
				 *  which might cause disk corruption
				 */
				*p_scratchcache = 0;
				return DFS_BAD_CLUS;
			}
			*p_scratchcache = sector;
			result |= ((uint32_t) p_scratch[0]) << 8;
		} else {
			result = (uint32_t) p_scratch[offset] |
			  ((uint32_t) p_scratch[offset+1]) << 8;
		}
		if (cluster & 1)
			result = result >> 4;
		else
			result = result & 0xfff;
	} else if (volinfo->filesystem == FAT16) {
		result = (uint32_t) p_scratch[offset] |
		  ((uint32_t) p_scratch[offset+1]) << 8;
	} else if (volinfo->filesystem == FAT32) {
		result = ((uint32_t) p_scratch[offset] |
		  ((uint32_t) p_scratch[offset+1]) << 8 |
		  ((uint32_t) p_scratch[offset+2]) << 16 |
		  ((uint32_t) p_scratch[offset+3]) << 24) & 0x0fffffff;
	} else
		result = DFS_BAD_CLUS;

	return result;
}

/*
 * Set FAT entry for specified cluster number
 * You must provide a scratch buffer for one sector (SECTOR_SIZE)
 * and a populated volinfo_t Returns DFS_ERRMISC for any error, otherwise
 * DFS_OK p_scratchcache should point to a UINT32. This variable caches the
 * physical sector number last read into the scratch buffer for performance
 * enhancement reasons.
 *
 * NOTE: This code is HIGHLY WRITE-INEFFICIENT, particularly for flash media.
 * Considerable performance gains can be realized by caching the sector.
 * However this is difficult to achieve on FAT12 without requiring 2 sector
 * buffers of scratch space, and it is a design requirement of this code to
 * operate on a single 512-byte scratch. If you are operating DOSFS over flash,
 * you are strongly advised to implement a writeback cache in your physical
 * I/O driver. This will speed up your code significantly and will
 * also conserve power and flash write life.
 */

uint32_t fat_set_fat_(struct fat_fs_info *fsi, uint8_t *p_scratch,
		uint32_t *p_scratchcache, uint32_t cluster, uint32_t new_contents) {
	uint32_t offset, sector, result;
	struct volinfo *volinfo = &fsi->vi;

	switch (volinfo->filesystem) {
		case FAT12:
			offset = cluster + (cluster / 2);
			new_contents &=0xfff;
			break;
		case FAT16:
			offset = cluster * 2;
			new_contents &=0xffff;
			break;
		case FAT32:
			offset = cluster * 4;
			new_contents &=0x0fffffff;	/* FAT32 is really "FAT28" */
			break;
		default:
			return DFS_ERRMISC;
	}
	/*
	 * at this point, offset is the BYTE offset of the desired sector from
	 * the start of the FAT.
	 * Calculate the physical sector containing this FAT entry.
	 */
	sector = offset / volinfo->bytepersec + volinfo->fat1;

	/* If this is not the same sector we last read, then read it into RAM */
	if (sector != *p_scratchcache) {
		if (fat_read_sector(fsi, p_scratch, sector)) {
			/*
			 * avoid anyone assuming that this cache value is still valid,
			 * which might cause disk corruption
			 */
			*p_scratchcache = 0;
			return DFS_ERRMISC;
		}
		*p_scratchcache = sector;
	}

	/*
	 * At this point, we "merely" need to extract the relevant entry.
	 * This is easy for FAT16 and FAT32, but a royal PITA for FAT12 as a
	 * single entry may span a sector boundary. The normal way around this
	 * is always to read two FAT sectors, but that luxury is (by design intent)
	 * unavailable to DOSFS.
	 */
	offset %= volinfo->bytepersec;

	switch (volinfo->filesystem) {
	case FAT12:
		if (cluster & 1)
			new_contents = new_contents << 4;

		if (offset == volinfo->bytepersec - 1) {
			/* Odd cluster: High 12 bits being set */
			if (cluster & 1) {
				p_scratch[offset] = (p_scratch[offset] & 0x0f) |
						(new_contents & 0xf0);
			}
			/* Even cluster: Low 12 bits being set */
			else {
				p_scratch[offset] = new_contents & 0xff;
			}
			result = fat_write_sector(fsi, p_scratch, *p_scratchcache);
			/* mirror the FAT into copy 2 */
			if (DFS_OK == result) {
				result = fat_write_sector(fsi, p_scratch,
						(*p_scratchcache) + volinfo->secperfat);
			}

			/*
			 * If we wrote that sector OK, then read in the subsequent sector
			 * and poke the first byte with the remainder of this FAT entry.
			 */
			if (DFS_OK == result) {
				(*p_scratchcache)++;
				result = fat_read_sector(fsi, p_scratch, *p_scratchcache);
				if (DFS_OK == result) {
					/* Odd cluster: High 12 bits being set*/
					if (cluster & 1) {
						p_scratch[0] = new_contents & 0xff00;
					}
					/* Even cluster: Low 12 bits being set */
					else {
						p_scratch[0] = (p_scratch[0] & 0xf0) |
								(new_contents & 0x0f);
					}
					result = fat_write_sector(fsi, p_scratch,
							*p_scratchcache);
					/* mirror the FAT into copy 2 */
					if (DFS_OK == result) {
						result = fat_write_sector(fsi, p_scratch,
								(*p_scratchcache)+volinfo->secperfat);
					}
				} else {
					/*
					 * avoid anyone assuming that this cache value is still
					 * valid, which might cause disk corruption
					 */
					*p_scratchcache = 0;
				}
			}
		}

		/*
		 * Not a sector boundary. But we still have to worry about if it's an
		 * odd or even cluster number.
		 */
		else {
			/* Odd cluster: High 12 bits being set */
			if (cluster & 1) {
				p_scratch[offset] = (p_scratch[offset] & 0x0f) |
						(new_contents & 0xf0);
				p_scratch[offset + 1] = (new_contents & 0xff00) >> 8;
			}
			/* Even cluster: Low 12 bits being set */
			else {
				p_scratch[offset] = new_contents & 0xff;
				p_scratch[offset+1] = (p_scratch[offset+1] & 0xf0) |
						((new_contents & 0x0f00) >> 8);
			}
			result = fat_write_sector(fsi, p_scratch, *p_scratchcache);
			/* mirror the FAT into copy 2 */
			if (DFS_OK == result) {
				result = fat_write_sector(fsi, p_scratch,
						(*p_scratchcache) + volinfo->secperfat);
			}
		}
		break;
	case FAT32:
		p_scratch[offset + 3] = (p_scratch[offset  + 3] & 0xf0) |
				((new_contents & 0x0f000000) >> 24);
		p_scratch[offset + 2] = (new_contents & 0xff0000) >> 16;
		/* Fall through */
	case FAT16:
		p_scratch[offset + 1] = (new_contents & 0xff00) >> 8;
		p_scratch[offset] = (new_contents & 0xff);
		result = fat_write_sector(fsi, p_scratch, *p_scratchcache);
		/* mirror the FAT into copy 2 */
		if (DFS_OK == result)
			result = fat_write_sector(
					fsi,
					p_scratch,
					(*p_scratchcache) + volinfo->secperfat
				);
		break;
	default:
		result = DFS_ERRMISC;
	}
	return result;
}

/* For long names string is divided in a pretty ugly way so old drivers
 * will be able to read directory content, so we need some ugly code to
 * figure it out. NOTE: we support only ASCII-charaters filenames */
static void fat_append_longname(char *name, struct fat_dirent *di) {
	struct fat_long_dirent *ld;
	const int chars_per_long_entry = 13;
	int l;

	assert(name);
	assert(di);

	ld = (void *) di;

	l = chars_per_long_entry * ((di->name[0] & FAT_LONG_ORDER_NUM_MASK) - 1);

	name[l++] = (char) ld->name1[0];
	name[l++] = (char) ld->name1[2];
	name[l++] = (char) ld->name1[4];
	name[l++] = (char) ld->name1[6];
	name[l++] = (char) ld->name1[8];

	name[l++] = (char) ld->name2[0];
	name[l++] = (char) ld->name2[2];
	name[l++] = (char) ld->name2[4];
	name[l++] = (char) ld->name2[6];
	name[l++] = (char) ld->name2[8];
	name[l++] = (char) ld->name2[10];

	name[l++] = (char) ld->name3[0];
	name[l++] = (char) ld->name3[2];

	if (di->name[0] & FAT_LONG_ORDER_LAST) {
		name[l] = '\0';
	}
}

/*
 * 	Find the first unused FAT entry
 * 	You must provide a scratch buffer for one sector (SECTOR_SIZE) and a
 * 	populated volinfo_t Returns a FAT32 BAD_CLUSTER value for any error,
 * 	otherwise the contents of the desired FAT entry.
 * 	Returns FAT32 bad_sector (0x0ffffff7) if there is no free cluster available
 */
uint32_t fat_get_free_fat_(struct fat_fs_info *fsi, uint8_t *p_scratch) {
	uint32_t i, result = 0xffffffff, p_scratchcache = 0;
	/*
	 * Search starts at cluster 2, which is the first usable cluster
	 * NOTE: This search can't terminate at a bad cluster, because there might
	 * legitimately be bad clusters on the disk.
	 */
	for (i = 2; i < fsi->vi.numclusters; i++) {
		result = fat_get_fat_(fsi, p_scratch, &p_scratchcache, i);
		if (!result)
			return i;
	}
	return DFS_BAD_CLUS;
}

static inline int dir_is_root(uint8_t *name) {
	return !strlen((char *) name) ||
		((strlen((char *) name) == 1) && (name[0] == DIR_SEPARATOR));
}

/*
 * Open a directory for enumeration by fat_get_nextDirEnt
 * You must supply a populated volinfo_t (see fat_get_volinfo)
 * The empty string or a string containing only the directory separator are
 * considered to be the root directory.
 * Returns 0 OK, nonzero for any error.
 */
uint32_t fat_open_dir(struct fat_fs_info *fsi,
		uint8_t *dirname, struct dirinfo *dirinfo) {
	struct volinfo *volinfo;

	assert(fsi);

	volinfo = &fsi->vi;
	dirinfo->flags = 0;
	dirinfo->currentsector = 0;
	dirinfo->currententry = 0;

	if (dir_is_root(dirname)) {
		uint32_t ret;
		dirinfo->currentcluster = volinfo->rootdir / volinfo->secperclus;
		if (volinfo->filesystem == FAT32) {
			dirinfo->currentsector = volinfo->dataarea;
			dirinfo->currentsector += ((volinfo->rootdir - 2) * volinfo->secperclus);
		} else {
			dirinfo->currentsector = (volinfo->rootdir % volinfo->secperclus);
		}

		ret = fat_read_sector(fsi, dirinfo->p_scratch,
				dirinfo->currentsector + dirinfo->currentcluster * volinfo->secperclus);

		dirinfo->currentsector = volinfo->rootdir;
		dirinfo->currentcluster = 2;

		return ret;
	} else {
		uint8_t tmpfn[12];
		uint8_t *ptr = dirname;
		uint32_t result;
		struct fat_dirent de;

		if (volinfo->filesystem == FAT32) {
			dirinfo->currentcluster = volinfo->rootdir / volinfo->secperclus;
			if (fat_read_sector(fsi, dirinfo->p_scratch, volinfo->dataarea +
					((volinfo->rootdir - 2) * volinfo->secperclus))) {
				return DFS_ERRMISC;
			}
		} else {
			dirinfo->currentcluster = 0;
			if (fat_read_sector(fsi, dirinfo->p_scratch, volinfo->rootdir)) {
				return DFS_ERRMISC;
			}
		}

		while (*ptr == DIR_SEPARATOR && *ptr) {
			ptr++;
		}

		/*
		 *  Scan the path from left to right, finding the start cluster
		 *  of each entry.  Observe that this code is inelegant, but obviates
		 *  the need for recursion.
		 */
		while (*ptr) {
			path_canonical_to_dir((char *) tmpfn, (char *) ptr);

			de.name[0] = '\0';

			do {
				result = fat_get_next(fsi, dirinfo, &de);
			} while (!result && memcmp(de.name, tmpfn, MSDOS_NAME));

			if (!memcmp(de.name, tmpfn, MSDOS_NAME) && (de.attr & ATTR_DIRECTORY)) {
				if (volinfo->filesystem == FAT32) {
					dirinfo->currentcluster = (uint32_t) de.startclus_l_l |
					  ((uint32_t) de.startclus_l_h) << 8 |
					  ((uint32_t) de.startclus_h_l) << 16 |
					  ((uint32_t) de.startclus_h_h) << 24;
				}
				else {
					dirinfo->currentcluster = (uint32_t) de.startclus_l_l |
					  ((uint32_t) de.startclus_l_h) << 8;
				}
				dirinfo->currentsector = 0;
				dirinfo->currententry = 0;

				if (fat_read_sector(fsi, dirinfo->p_scratch, volinfo->dataarea +
					((dirinfo->currentcluster - 2) * volinfo->secperclus))) {
					return DFS_ERRMISC;
				}
			} else if (!memcmp(de.name, tmpfn, MSDOS_NAME) && !(de.attr & ATTR_DIRECTORY))
				return DFS_WRONGRES;

			while (*ptr != DIR_SEPARATOR && *ptr)
				ptr++;

			while (*ptr == DIR_SEPARATOR)
				ptr++;
		}

		if (!dirinfo->currentcluster)
			return DFS_NOTFOUND;
	}

	return DFS_OK;
}

/*
 * Get next entry in opened directory structure.
 * Copies fields into the dirent structure, updates dirinfo. Note that it is
 * the _caller's_ responsibility to	handle the '.' and '..' entries.
 * A deleted file will be returned as a NULL entry (first char of filename=0)
 * by this code. Filenames beginning with 0x05 will be translated to 0xE5
 * automatically. Long file name entries will be returned as NULL.
 * returns DFS_EOF if there are no more entries, DFS_OK if this entry is valid,
 * or DFS_ERRMISC for a media error
 */
uint32_t fat_get_next(struct fat_fs_info *fsi,
		struct dirinfo *dir, struct fat_dirent *dirent) {
	struct volinfo *volinfo;
	struct fat_dirent *dirent_src;
	uint32_t tempint;
	int next_ent;
	int ent_per_sec;
	int end_of_chain;
	int read_sector;

	volinfo = &fsi->vi;
	ent_per_sec = volinfo->bytepersec / sizeof(struct fat_dirent);
	dirent->name[0] = '\0';

	/* Do we need to read the next sector of the directory? */
	if (dir->currententry >= ent_per_sec) {
		dir->currententry = 0;
		dir->currentsector++;

		/* Root directory; special case handling
		 * Note that currentcluster will only ever be zero if both:
		 * (a) this is the root directory, and
		 * (b) we are on a FAT12/16 volume, where the root dir can't be
		 * expanded
		 */
		if (dir->currentcluster == 0) {
			/* Trying to read past end of root directory? */
			next_ent  = dir->currentsector * volinfo->bytepersec;
			next_ent /= sizeof(struct fat_dirent);
			if (next_ent >= volinfo->rootentries)
				return DFS_EOF;

			/* Otherwise try to read the next sector */
			if (fat_read_sector(fsi, dir->p_scratch, dir->currentsector))
				return DFS_ERRMISC;
		} else {
			if (dir->currentsector >= volinfo->secperclus) {
				dir->currentsector = 0;
				switch (volinfo->filesystem) {
				case FAT12:
					end_of_chain = dir->currentcluster >= 0xff7;
					break;
				case FAT16:
					end_of_chain = dir->currentcluster >= 0xfff7;
					break;
				case FAT32:
					end_of_chain = dir->currentcluster >= 0xffffff7;
					break;
				default:
					return DFS_ERRMISC;
				}

				if (end_of_chain) {
					/* We are at the end of the directory chain.
					 * If this is a normal find operation, we should indicate
					 * that there is nothing more to see.
					 */
					if (!(dir->flags & DFS_DI_BLANKENT))
						return DFS_EOF;
					/*
					 * On the other hand, if this is a "find free entry"
					 * search, we need to tell the caller to allocate a
					 * new cluster
					 */
					else
						return DFS_ALLOCNEW;
				}

				dir->currentcluster = fat_get_fat_(fsi,
						dir->p_scratch,
						&tempint,
						dir->currentcluster);
			}

			read_sector  = (dir->currentcluster - 2) * volinfo->secperclus;
			read_sector += volinfo->dataarea + dir->currentsector;

			if (fat_read_sector(fsi, dir->p_scratch, read_sector))
				return DFS_ERRMISC;
		}
	}

	dirent_src = &((struct fat_dirent *) dir->p_scratch)[dir->currententry];
	memcpy(dirent, dirent_src, sizeof(struct fat_dirent));

	if (dirent->name[0] == '\0') {
		if (dir->flags & DFS_DI_BLANKENT) {
			dir->currententry++; // DOSFS 1.03 BUG, currententry was not incremented in this case
			return DFS_OK;
		} else
			return DFS_EOF;
	}

	if (dirent->name[0] == 0xe5) {
		dirent->name[0] = '\0';
	} else if (dirent->name[0] == 0x05)
		/* handle kanji filenames beginning with 0xE5 */
		dirent->name[0] = 0xe5;

	dir->currententry++;

	return DFS_OK;
}

static inline int dirinfo_is_root(struct volinfo *vi, struct dirinfo *di) {
	assert(vi);
	assert(di);

	if (vi->filesystem == FAT32) {
		return di->currentsector == 0 && di->currentcluster == 2;
	} else {
		return di->currentsector == (vi->rootdir % vi->secperclus);
	}
}
/* Same as fat_get_next(), but skip long-name entries with following 8.3-entries */
uint32_t fat_get_next_long(struct fat_fs_info *fsi, struct dirinfo *dir,
		struct fat_dirent *dirent, char *name_buf) {
	uint32_t ret;

	assert(fsi);
	assert(dir);
	assert(dir->p_scratch);
	assert(dirent);

	ret = fat_get_next(fsi, dir, dirent);

	if (dirent->attr != ATTR_LONG_NAME) {
		if (name_buf != NULL) {
			strncpy(name_buf, (char *) dirent->name, MSDOS_NAME);
		}
	} else {
		/* Long name entry */
		while (dirent->attr == ATTR_LONG_NAME) {
			if (name_buf != NULL) {
				fat_append_longname(name_buf, dirent);
			}
			ret = fat_get_next(fsi, dir, dirent);
		}
		/* Now cur_di points to 8.3 entry which corresponds to current
		 * file, so we need to get next entry for a next file */
	}

	return ret;

}

/*
 * INTERNAL
 * Find a free directory entry in the directory specified by path
 * This function MAY cause a disk write if it is necessary to extend the
 * directory size.
 * Note - di.p_scratch must be preinitialized to point to a sector scratch buffer
 * de is a scratch structure
 * Returns DFS_ERRMISC if a new entry could not be located or created
 * de is updated with the same return information you would expect
 * from fat_get_next
 */
uint32_t fat_get_free_dir_ent(struct fat_fs_info *fsi, uint8_t *path,
		struct dirinfo *di, struct fat_dirent *de) {
	uint32_t tempclus,i;
	struct volinfo *volinfo;

	volinfo = &fsi->vi;

	//if (fat_open_dir(fsi, path, di)) {
	//	return DFS_NOTFOUND;
	//}

	di->flags |= DFS_DI_BLANKENT;

	/*
	 * Note we are reusing tempclus as a temporary result holder.
	 */
	tempclus = 0;
	do {
		tempclus = fat_get_next(fsi, di, de);

		if (tempclus == DFS_OK && (!de->name[0])) {
			return DFS_OK;
		} else if (tempclus == DFS_EOF) {
			return DFS_ERRMISC;
		} else if (tempclus == DFS_ALLOCNEW) {
			tempclus = fat_get_free_fat_(fsi, di->p_scratch);
			if (tempclus == DFS_BAD_CLUS) {
				return DFS_ERRMISC;
			}

			memset(di->p_scratch, 0, volinfo->bytepersec);
			for (i = 0; i < volinfo->secperclus; i++) {
				if (fat_write_sector(fsi, di->p_scratch,
						volinfo->dataarea + ((tempclus - 2) *
								volinfo->secperclus) + i)) {
					return DFS_ERRMISC;
				}
			}
			i = 0;
			fat_set_fat_(fsi,
					di->p_scratch, &i, di->currentcluster, tempclus);

			di->currentcluster = tempclus;
			di->currentsector = 0;
			di->currententry = 0; /* tempclus is not zero but contains fat entry, so next loop will call */
			switch(volinfo->filesystem) {
				case FAT12:		tempclus = 0xfff;	break;
				case FAT16:		tempclus = 0xffff;	break;
				case FAT32:		tempclus = 0x0fffffff;	break;
				default:		return DFS_ERRMISC;
			}
			fat_set_fat_(fsi, di->p_scratch, &i, di->currentcluster, tempclus);
		}
	} while (!tempclus);

	/* We shouldn't get here */
	return DFS_ERRMISC;
}

void fat_set_direntry (uint32_t dir_cluster, uint32_t cluster) {
	struct fat_dirent *de = (struct fat_dirent *) fat_sector_buff;

	de[0] = (struct fat_dirent) {
		.name = MSDOS_DOT,
		.attr = ATTR_DIRECTORY,
		.startclus_l_l = cluster & 0xff,
		.startclus_l_h = (cluster & 0xff00) >> 8,
		.startclus_h_l = (cluster & 0xff0000) >> 16,
		.startclus_h_h = (cluster & 0xff000000) >> 24,
	};

	de[1] = (struct fat_dirent) {
		.name = MSDOS_DOTDOT,
		.attr = ATTR_DIRECTORY,
		.startclus_l_l = dir_cluster & 0xff,
		.startclus_l_h = (dir_cluster & 0xff00) >> 8,
		.startclus_h_l = (dir_cluster & 0xff0000) >> 16,
		.startclus_h_h = (dir_cluster & 0xff000000) >> 24,
	};

	fat_set_filetime(&de[0]);
	fat_set_filetime(&de[1]);
}

int fat_root_dir_record(void *bdev) {
	uint32_t cluster;
	struct fat_fs_info fsi;
	uint32_t pstart, psize;
	uint8_t pactive, ptype;
	struct fat_dirent de;
	int dev_blk_size = bdev_blk_sz(bdev);
	int root_dir_sz;

	assert(dev_blk_size > 0);

	fsi.bdev = bdev;

	/* Obtain pointer to first partition on first (only) unit */
	pstart = fat_get_ptn_start(bdev, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		return -1;
	}

	if (fat_get_volinfo(bdev, &fsi.vi, pstart)) {
		return -1;
	}

	cluster = fsi.vi.rootdir / fsi.vi.secperclus;

	de = (struct fat_dirent) {
		.name = "ROOT DIR   ",
		.attr = ATTR_DIRECTORY,
		.startclus_l_l = cluster & 0xff,
		.startclus_l_h = (cluster & 0xff00) >> 8,
		.startclus_h_l = (cluster & 0xff0000) >> 16,
		.startclus_h_h = (cluster & 0xff000000) >> 24,
	};

	fat_set_filetime(&de);

	/*
	 * write the directory entry
	 * note that we no longer have the sector containing the directory
	 * entry, tragically, so we have to re-read it
	 */

	/* we clear other FAT TABLE */
	memset(fat_sector_buff, 0, sizeof(fat_sector_buff));
	memcpy(&(((struct fat_dirent*) fat_sector_buff)[0]), &de, sizeof(struct fat_dirent));

	if (0 > block_dev_write(	bdev,
					(char *) fat_sector_buff,
					fsi.vi.bytepersec,
					fsi.vi.rootdir * fsi.vi.bytepersec / dev_blk_size)) {
		return DFS_ERRMISC;
	}

	root_dir_sz = (fsi.vi.rootentries * sizeof(struct fat_dirent) +
	               fsi.vi.bytepersec - 1) / fsi.vi.bytepersec - 1;

	if (root_dir_sz)
		memset(fat_sector_buff, 0, sizeof(struct fat_dirent)); /* The rest is zeroes already */
	/* Clear the rest of root directory */
	while (root_dir_sz) {
		block_dev_write(bdev,
				(char *) fat_sector_buff,
				fsi.vi.bytepersec,
				(root_dir_sz + fsi.vi.rootdir) * fsi.vi.bytepersec / dev_blk_size);
		root_dir_sz--;
	}


	/* Mark newly allocated cluster as end of chain */
	switch (fsi.vi.filesystem) {
		case FAT12:		cluster = 0xfff;	break;
		case FAT16:		cluster = 0xffff;	break;
		case FAT32:		cluster = 0x0fffffff;	break;
		default:		return DFS_ERRMISC;
	}
	psize = 0;
	fat_set_fat_(&fsi, fat_sector_buff, &psize, cluster, cluster);

	return DFS_OK;
}

/*
 * Read an open file
 * You must supply a prepopulated file_info_t as provided by fat_open_file,
 * and a pointer to a volinfo->bytepersec scratch buffer.
 * 	Note that returning DFS_EOF is not an error condition. This function
 * 	updates the	successcount field with the number of bytes actually read.
 */
uint32_t fat_read_file(struct fat_file_info *fi, uint8_t *p_scratch,
		uint8_t *buffer, uint32_t *successcount, uint32_t len) {
	uint32_t remain;
	uint32_t result;
	uint32_t sector;
	uint32_t bytesread;
	uint32_t clastersize;
	struct fat_fs_info *fsi;
	fsi = fi->fsi;

	result = DFS_OK;
	remain = len;
	*successcount = 0;
	clastersize = fi->volinfo->secperclus * fi->volinfo->bytepersec;

	while (remain && result == DFS_OK) {
		/* This is a bit complicated. The sector we want to read is addressed
		 * at a cluster granularity by the fi->cluster member. The file
		 * pointer tells us how many extra sectors to add to that number.
		 */
		sector = fi->volinfo->dataarea +
		  ((fi->cluster - 2) * fi->volinfo->secperclus) +
		  div(div(fi->pointer, clastersize).rem, fi->volinfo->bytepersec).quot;

		/* Case 1 - File pointer is not on a sector boundary */
		if (div(fi->pointer, fi->volinfo->bytepersec).rem) {
			uint16_t tempreadsize;

			/* We always have to go through scratch in this case */
			result = fat_read_sector(fsi, p_scratch, sector);

			/*
			 * This is the number of bytes that we actually care about in the
			 * sector just read.
			 */
			tempreadsize = fi->volinfo->bytepersec -
					(div(fi->pointer, fi->volinfo->bytepersec).rem);

			/* Case 1A - We want the entire remainder of the sector. After this
			 * point, all passes through the read loop will be aligned on a
			 * sector boundary, which allows us to go through the optimal path
			 *  2A below.
			 */
		   	if (remain >= tempreadsize) {
				memcpy(buffer, p_scratch + (fi->volinfo->bytepersec - tempreadsize),
						tempreadsize);
				bytesread = tempreadsize;
				buffer += tempreadsize;
				fi->pointer += tempreadsize;
				remain -= tempreadsize;
			}
			/* Case 1B - This read concludes the file read operation */
			else {
				memcpy(buffer, p_scratch +
						(fi->volinfo->bytepersec - tempreadsize), remain);

				buffer += remain;
				fi->pointer += remain;
				bytesread = remain;
				remain = 0;
			}
		}
		/* Case 2 - File pointer is on sector boundary */
		else {
			/*
			 * Case 2A - We have at least one more full sector to read and
			 * don't have to go through the scratch buffer. You could insert
			 * optimizations here to read multiple sectors at a time, if you
			 * were thus inclined (note that the maximum multi-read you could
			 * perform is a single cluster, so it would be advantageous to have
			 * code similar to case 1A above that would round the pointer to a
			 * cluster boundary the first pass through, so all subsequent
			 * [large] read requests would be able to go a cluster at a time).
			 */
			 if (remain >= fi->volinfo->bytepersec) {
				result = fat_read_sector(fsi, buffer, sector);
				remain -= fi->volinfo->bytepersec;
				buffer += fi->volinfo->bytepersec;
				fi->pointer += fi->volinfo->bytepersec;
				bytesread = fi->volinfo->bytepersec;
			}
			/* Case 2B - We are only reading a partial sector */
			else {
				result = fat_read_sector(fsi, p_scratch, sector);
				memcpy(buffer, p_scratch, remain);
				buffer += remain;
				fi->pointer += remain;
				bytesread = remain;
				remain = 0;
			}
		}

		*successcount += bytesread;
		/* check to see if we stepped over a cluster boundary */
		if (div(fi->pointer - bytesread, clastersize).quot !=
			div(fi->pointer, clastersize).quot) {
			/*
			 * An act of minor evil - we use bytesread as a scratch integer,
			 * knowing that its value is not used after updating *successcount
			 * above
			 */
			bytesread = 0;
			if (((fi->volinfo->filesystem == FAT12) &&
					(fi->cluster >= 0xff8)) ||
					((fi->volinfo->filesystem == FAT16) &&
							(fi->cluster >= 0xfff8)) ||
							((fi->volinfo->filesystem == FAT32) &&
									(fi->cluster >= 0x0ffffff8))) {
				result = DFS_EOF;
			}
			else {
				fi->cluster = fat_get_fat_(fsi, p_scratch, &bytesread, fi->cluster);
			}
		}
	}

	return result;
}


/*
 * Write an open file
 * You must supply a prepopulated file_info_t as provided by
 * fat_open_file, and a pointer to a SECTOR_SIZE scratch buffer.
 * This function updates the successcount field with the number
 * of bytes actually written.
 */
uint32_t fat_write_file(struct fat_file_info *fi, uint8_t *p_scratch,
		uint8_t *buffer, uint32_t *successcount, uint32_t len, size_t *size) {
	uint32_t remain;
	uint32_t result = DFS_OK;
	uint32_t sector;
	uint32_t byteswritten;
	uint32_t lastcluster, nextcluster;
	uint32_t clastersize;
	struct fat_fs_info *fsi;
	fsi = fi->fsi;

	if (!(fi->mode & O_WRONLY) && !(fi->mode & O_APPEND) && !(fi->mode & O_RDWR)) {
		return DFS_ERRMISC;
	}

	remain = len;
	*successcount = 0;
	clastersize = fi->volinfo->secperclus * fi->volinfo->bytepersec;

	while (remain && result == DFS_OK) {
		/*
		 * This is a bit complicated. The sector we want to read is addressed
		 * at a cluster granularity by  the fi->cluster member.
		 * The file pointer tells us how many extra sectors to add to that
		 * number.
		 */
		sector = fi->volinfo->dataarea +
		  ((fi->cluster - 2) * fi->volinfo->secperclus) +
		  div(div(fi->pointer, clastersize).rem, fi->volinfo->bytepersec).quot;

		/* Case 1 - File pointer is not on a sector boundary */
		if (div(fi->pointer, fi->volinfo->bytepersec).rem) {
			uint16_t tempsize;

			/* We always have to go through scratch in this case */
			result = fat_read_sector(fsi, p_scratch, sector);

			/*
			 * This is the number of bytes that we don't want to molest in the
			 * scratch sector just read.
			 */
			tempsize = div(fi->pointer, fi->volinfo->bytepersec).rem;

			/*
			 * Case 1A - We are writing the entire remainder of the sector.
			 * After this point, all passes through the read loop will be
			 * aligned on a sector boundary, which allows us to go through the
			 * optimal path
			 * 2A below.
			 */
		   	if (remain >= fi->volinfo->bytepersec - tempsize) {
				memcpy(p_scratch + tempsize, buffer, fi->volinfo->bytepersec - tempsize);
				if (!result) {
					result = fat_write_sector(fsi, p_scratch, sector);
				}

				byteswritten = fi->volinfo->bytepersec - tempsize;
				buffer += fi->volinfo->bytepersec - tempsize;
				fi->pointer += fi->volinfo->bytepersec - tempsize;
				if (*size < fi->pointer) {
					*size = fi->pointer;
				}
				remain -= fi->volinfo->bytepersec - tempsize;
			}
			/* Case 1B - This concludes the file write operation */
			else {
				memcpy(p_scratch + tempsize, buffer, remain);
				if (!result) {
					result = fat_write_sector(fsi, p_scratch, sector);
				}

				buffer += remain;
				fi->pointer += remain;
				if (*size < fi->pointer) {
					*size = fi->pointer;
				}
				byteswritten = remain;
				remain = 0;
			}
		} /* case 1 */
		/* Case 2 - File pointer is on sector boundary */
		else {
			/* Case 2A - We have at least one more full sector to write and
			 * don't have to go through the scratch buffer. You could insert
			 * optimizations here to write multiple sectors at a time, if you
			 * were thus inclined. Refer to similar notes in fat_read_file.
			 */
			if (remain >= fi->volinfo->bytepersec) {
				result = fat_write_sector(fsi, buffer, sector);
				remain -= fi->volinfo->bytepersec;
				buffer += fi->volinfo->bytepersec;
				fi->pointer += fi->volinfo->bytepersec;
				if (*size < fi->pointer) {
					*size = fi->pointer;
				}
				byteswritten = fi->volinfo->bytepersec;
			}
			/*
			 * Case 2B - We are only writing a partial sector and potentially
			 * need to go through the scratch buffer.
			 */
			else {
				/* If the current file pointer is not yet at or beyond the file
				 * length, we are writing somewhere in the middle of the file
				 * and need to load the original sector to do
				 * a read-modify-write.
				 */
				if (fi->pointer < *size) {
					result = fat_read_sector(fsi, p_scratch, sector);
					if (!result) {
						memcpy(p_scratch, buffer, remain);
						result = fat_write_sector(fsi, p_scratch, sector);
					}
				}
				else {
					memset(p_scratch, 0, fi->volinfo->bytepersec);
					memcpy(p_scratch, buffer, remain);
					result = fat_write_sector(fsi, p_scratch, sector);
				}

				buffer += remain;
				fi->pointer += remain;
				if (*size < fi->pointer) {
					*size = fi->pointer;
				}
				byteswritten = remain;
				remain = 0;
			}
		}

		*successcount += byteswritten;

		/* check to see if we stepped over a cluster boundary */
		if (div(fi->pointer - byteswritten, clastersize).quot !=
				div(fi->pointer, clastersize).quot) {

		  	/* We've transgressed into another cluster. If we were already
		  	 * at EOF, we need to allocate a new cluster.
		  	 * An act of minor evil - we use byteswritten as a scratch integer,
		  	 * knowing that its value is not used after updating *successcount
		  	 * above
		  	 */
		  	byteswritten = 0;

			lastcluster = fi->cluster;
			fi->cluster = fat_get_fat_(fsi, p_scratch,
					&byteswritten, fi->cluster);

			/* Allocate a new cluster? */
			if (((fi->volinfo->filesystem == FAT12) &&
					(fi->cluster >= 0xff8)) ||
					((fi->volinfo->filesystem == FAT16) &&
					(fi->cluster >= 0xfff8)) ||
					((fi->volinfo->filesystem == FAT32) &&
					(fi->cluster >= 0x0ffffff8))) {
			  	uint32_t tempclus;
				tempclus = fat_get_free_fat_(fsi, p_scratch);
				byteswritten = 0; /* invalidate cache */
				if (tempclus == DFS_BAD_CLUS)
					return DFS_ERRMISC;
				/* Link new cluster onto file */
				fat_set_fat_(fsi, p_scratch,
						&byteswritten, lastcluster, tempclus);
				fi->cluster = tempclus;

				/* Mark newly allocated cluster as end of chain */
				switch(fi->volinfo->filesystem) {
					case FAT12:		tempclus = 0xfff;	break;
					case FAT16:		tempclus = 0xffff;	break;
					case FAT32:		tempclus = 0x0fffffff;	break;
					default:		return DFS_ERRMISC;
				}
				fat_set_fat_(fsi, p_scratch,
						&byteswritten, fi->cluster, tempclus);

				result = DFS_OK;
			}
		}
	}
	/* If cleared, then mark free clusters*/
	// TODO implement fat truncate
	if (0 && *size > fi->pointer) {
		if (div(*size, clastersize).quot !=
			div(fi->pointer, clastersize).quot) {

			byteswritten = 0;/* invalidate cache */
			nextcluster = fat_get_fat_(fsi, p_scratch,
					&byteswritten, fi->cluster);

			switch(fi->volinfo->filesystem) {
				case FAT12:		lastcluster = 0xfff;	break;
				case FAT16:		lastcluster = 0xfff;	break;
				case FAT32:		lastcluster = 0x0fffffff;	break;
				default:		return DFS_ERRMISC;
			}
			fat_set_fat_(fsi, p_scratch, &byteswritten,
					fi->cluster, lastcluster);

			/* Now follow the cluster chain to free the file space */
			while (!((fi->volinfo->filesystem == FAT12 &&
					nextcluster >= 0x0ff7) ||
					(fi->volinfo->filesystem == FAT16 &&
					nextcluster >= 0xfff7) ||
					(fi->volinfo->filesystem == FAT32 &&
					nextcluster >= 0x0ffffff7))) {
				lastcluster = nextcluster;
				nextcluster = fat_get_fat_(fsi, p_scratch,
						&byteswritten, nextcluster);

				fat_set_fat_(fsi, p_scratch,
						&byteswritten, lastcluster, 0);
			}

		}
	}

	//*size = fi->pointer; // TODO implement fat truncate

	/* Update directory entry */
	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	((struct fat_dirent*) p_scratch)[fi->diroffset].filesize_0 =
			 *size & 0xff;

	((struct fat_dirent*) p_scratch)[fi->diroffset].filesize_1 =
			(*size & 0xff00) >> 8;

	((struct fat_dirent*) p_scratch)[fi->diroffset].filesize_2 =
			(*size & 0xff0000) >> 16;

	((struct fat_dirent*) p_scratch)[fi->diroffset].filesize_3 =
			(*size & 0xff000000) >> 24;

	if (fat_write_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	return result;
}

/*
 * Open a file for reading or writing. You supply populated volinfo_t,
 * a path to the file, mode (DFS_READ or DFS_WRITE) and an empty fileinfo
 * structure. You also need to provide a pointer to a sector-sized scratch
 * buffer.
 * Returns various DFS_* error states. If the result is DFS_OK, fileinfo
 * can be used to access the file from this point on.
 */
uint32_t fat_open_file(struct fat_file_info *fi, uint8_t *path, int mode,
		uint8_t *p_scratch, size_t *size) {
	char tmppath[PATH_MAX];
	char *filename;
	struct dirinfo di;
	struct fat_dirent de;

	struct volinfo *volinfo;
	struct fat_fs_info *fsi;

	assert(fi);
	assert(path);
	assert(p_scratch);
	assert(size);

	memset(tmppath, 0, sizeof(tmppath));
	filename = strrchr((char *) path, DIR_SEPARATOR);
	if (filename == NULL) {
		strcpy(tmppath, ROOT_DIR);
		filename = (char *) path;
	} else {
		strncpy(tmppath, (char *) path, filename - (char *) path);
		filename++;
	}

	fsi = fi->fsi;
	volinfo = &fsi->vi;

	fi->mode = mode;

	/*
	 *  At this point, if our path was MYDIR/MYDIR2/FILE.EXT,
	 *  filename = "FILE    EXT" and  tmppath = "MYDIR/MYDIR2".
	 */
	di.p_scratch = p_scratch;
	if (fat_open_dir(fsi, (uint8_t *) tmppath, &di)) {
		return DFS_NOTFOUND;
	}

	while (!fat_get_next_long(fsi, &di, &de, tmppath)) {
		if (!strcmp(tmppath, filename)) {
			if (de.attr & ATTR_DIRECTORY){
				return DFS_WRONGRES;
			}

			fi->volinfo = volinfo;
			fi->pointer = 0;
			/*
			 * The reason we store this extra info about the file is so that we
			 * can speedily update the file size, modification date, etc. on a
			 * file that is opened for writing.
			 */
			if (di.currentcluster == 0) {
				fi->dirsector = volinfo->rootdir + di.currentsector;
			} else {
				fi->dirsector = volinfo->dataarea +
						((di.currentcluster - 2) *
						volinfo->secperclus) + di.currentsector;
			}
			fi->diroffset = di.currententry - 1;
			if (volinfo->filesystem == FAT32) {
				fi->cluster = (uint32_t) de.startclus_l_l |
				  ((uint32_t) de.startclus_l_h) << 8 |
				  ((uint32_t) de.startclus_h_l) << 16 |
				  ((uint32_t) de.startclus_h_h) << 24;
			} else {
				fi->cluster = (uint32_t) de.startclus_l_l |
				  ((uint32_t) de.startclus_l_h) << 8;
			}
			fi->firstcluster = fi->cluster;
			fi->filelen = (uint32_t) de.filesize_0 |
			              ((uint32_t) de.filesize_1) << 8 |
			              ((uint32_t) de.filesize_2) << 16 |
			              ((uint32_t) de.filesize_3) << 24;
			if (size)
				*size = fi->filelen;
			return DFS_OK;
		}
	}
	return DFS_NOTFOUND;
}

/*
 * Delete a file
 * p_scratch must point to a sector-sized buffer
 */
int fat_unlike_file(struct fat_file_info *fi, uint8_t *path,
		uint8_t *p_scratch) {
	uint32_t cache;
	uint32_t tempclus;
	struct volinfo *volinfo;
	struct fat_fs_info *fsi;

	fsi = fi->fsi;
	volinfo = &fsi->vi;
	cache = 0;

	/* if (DFS_OK != fat_open_file(fi, path, O_RDONLY, p_scratch, NULL)) {
		return DFS_NOTFOUND;
	} */

	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	((struct fat_dirent*) p_scratch)[fi->diroffset].name[0] = 0xe5;
	if (fat_write_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	/* Now follow the cluster chain to free the file space */
	while (!((volinfo->filesystem == FAT12 && fi->firstcluster >= 0x0ff7) ||
			(volinfo->filesystem == FAT16 && fi->firstcluster >= 0xfff7) ||
			(volinfo->filesystem == FAT32 && fi->firstcluster >= 0x0ffffff7))) {
		tempclus = fi->firstcluster;
		//assert(fsi->bdev == nas->fs->bdev);
		fi->firstcluster = fat_get_fat_(fsi, p_scratch,
				&cache, fi->firstcluster);
		//assert(nas->fs->bdev == fsi->bdev);
		fat_set_fat_(fsi, p_scratch, &cache, tempclus, 0);
	}
	return DFS_OK;
}

/*
 * Delete a file
 * p_scratch must point to a sector-sized buffer
 */
int fat_unlike_directory(struct fat_file_info *fi, uint8_t *path,
		uint8_t *p_scratch) {
	uint32_t cache;
	uint32_t tempclus;
	struct volinfo *volinfo;
	struct fat_fs_info *fsi;

	fsi = fi->fsi;
	volinfo = &fsi->vi;

	cache = 0;

	/* fat_open_file gives us all the information we need to delete it */
	/* if (DFS_OK != fat_open_file(fi, path, O_RDONLY, p_scratch, NULL)) {
		return DFS_NOTFOUND;
	} */

	/* First, read the directory sector and delete that entry */
	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	((struct fat_dirent*) p_scratch)[fi->diroffset].name[0] = 0xe5;
	if (fat_write_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	/* Now follow the cluster chain to free the file space */
	while (!((volinfo->filesystem == FAT12 && fi->firstcluster >= 0x0ff7) ||
			(volinfo->filesystem == FAT16 && fi->firstcluster >= 0xfff7) ||
			(volinfo->filesystem == FAT32 && fi->firstcluster >= 0x0ffffff7))) {
		tempclus = fi->firstcluster;
		//assert(nas->fs->bdev == fsi->bdev);
		fi->firstcluster = fat_get_fat_(fsi, p_scratch,
				&cache, fi->firstcluster);
		//assert(nas->fs->bdev == fsi->bdev);
		fat_set_fat_(fsi, p_scratch, &cache, tempclus, 0);
	}
	return DFS_OK;
}

/**
 * @brief Fill dirent with dirinfo data
 *
 * @param di Directory
 * @param de Dirent to be filled
 *
 * @return Negative error code or zero if succeed
 */
static int fat_dirent_by_file(struct fat_file_info *fi, struct fat_dirent *de) {
	struct fat_fs_info *fsi = fi->fsi;
	void *de_src;

	if (fat_read_sector(fsi, fat_sector_buff, fi->dirsector))
		return -1;

	de_src = &(((struct fat_dirent *)fat_sector_buff)[fi->diroffset]);
	memcpy(de, de_src, sizeof(struct fat_dirent));

	return 0;
}

/**
 * @brief Read directory info from drive and set currentcluster,
 * current sector
 *
 * @param di Directory to be updated. Di->fi.fsi should be set properly.
 *
 * @return Negative error code or zero if succeed
 */
int fat_reset_dir(struct dirinfo *di) {
	struct fat_dirent de = { }; /* Initialize with zeroes to fit -O2 */
	struct volinfo *vi;

	vi = &di->fi.fsi->vi;

	if (di->fi.dirsector == 0) {
		/* This is root dir */
		di->currentcluster = vi->rootdir / vi->secperclus;
		if (vi->filesystem == FAT32) {
			di->currentsector = 0;
			di->currentcluster = 2;
		} else {
			di->currentsector = (vi->rootdir % vi->secperclus);
			di->currentsector = vi->rootdir;
			di->currentcluster = 2;
		}

		di->currententry = 1;
	} else {
		fat_dirent_by_file(&di->fi, &de);

		if (vi->filesystem == FAT32) {
			di->currentcluster = (uint32_t) de.startclus_l_l |
			  ((uint32_t) de.startclus_l_h) << 8 |
			  ((uint32_t) de.startclus_h_l) << 16 |
			  ((uint32_t) de.startclus_h_h) << 24;
		} else {
			di->currentcluster = (uint32_t) de.startclus_l_l |
			  ((uint32_t) de.startclus_l_h) << 8;
		}
		di->currentsector = 0;
		di->currententry = 0;
		di->currententry = 0;
	}

	return 0;
}

/*
 * Create a file or directory. You supply a file_create_param_t
 * structure.
 * Returns various DFS_* error states. If the result is DFS_OK, file
 * was created and can be used.
 */
int fat_create_file(struct fat_file_info *fi, struct dirinfo *di, char *name, int mode) {
	uint8_t filename[12];
	struct fat_dirent de;
	struct volinfo *volinfo;
	uint32_t cluster, temp;
	struct fat_fs_info *fsi;
	int entries;
	uint32_t res;

	assert(fi);
	assert(di);
	assert(name);

	fsi = fi->fsi;
	assert(fsi);

	volinfo = &fsi->vi;
	fi->volinfo = volinfo;

	while (*name == '/') {
		name++;
	}

	entries = fat_entries_per_name(name);

	di->fi.fsi = fsi;
	fat_reset_dir(di);

	res = fat_get_free_entries(fsi, di, &de, entries);
	if (res != DFS_OK) {
		return -1;
	}

	if (entries > 1) {
		path_canonical_to_dir((char *) filename, name);
		/* Write long-name descriptors */
		for (int i = entries - 1; i >= 1; i--) {
			memset(&de, 0, sizeof(de));
			fat_write_longname(name + 13 * (i - 1), &de);
			de.attr = ATTR_LONG_NAME;
			de.crttimetenth = fat_canonical_name_checksum((char *) filename);
			de.name[0] = FAT_LONG_ORDER_NUM_MASK & i;
			if (i == entries - 1) {
				de.name[0] |= FAT_LONG_ORDER_LAST;
			}

			fat_write_de(fsi, di, &de);

			fat_get_next(fsi, di, &de);
		}
	}

	cluster = fat_get_free_fat_(fsi, fat_sector_buff);
	de = (struct fat_dirent) {
		.attr = S_ISDIR(mode) ? ATTR_DIRECTORY : 0,
		.startclus_l_l = cluster & 0xff,
		.startclus_l_h = (cluster & 0xff00) >> 8,
		.startclus_h_l = (cluster & 0xff0000) >> 16,
		.startclus_h_h = (cluster & 0xff000000) >> 24,
	};
	memcpy(de.name, filename, MSDOS_NAME);
	fat_set_filetime(&de);

	fi->volinfo = volinfo;
	fi->pointer = 0;
	/*
	 * The reason we store this extra info about the file is so that we can
	 * speedily update the file size, modification date, etc. on a file
	 * that is opened for writing.
	 */

	//fi->dirsector = volinfo->dataarea + (di->fi.cluster - 2) * volinfo->secperclus;
	/* 'di' have to be filled already */
	fi->dirsector = di->currentsector + di->currentcluster * volinfo->secperclus;
	fi->diroffset = di->currententry;
	fi->cluster = cluster;
	fi->firstcluster = cluster;

	/*
	 * write the directory entry
	 * note that we no longer have the sector containing the directory
	 * entry, tragically, so we have to re-read it
	 */

	if (fat_read_sector(fsi, fat_sector_buff, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	memcpy(&(((struct fat_dirent*) fat_sector_buff)[fi->diroffset]),
			&de, sizeof(struct fat_dirent));
	if (fat_write_sector(fsi, fat_sector_buff, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	/* Mark newly allocated cluster as end of chain */
	switch(volinfo->filesystem) {
		case FAT12:		cluster = 0xfff;	break;
		case FAT16:		cluster = 0xffff;	break;
		case FAT32:		cluster = 0x0fffffff;	break;
		default:		return DFS_ERRMISC;
	}

	temp = 0;
	fat_set_fat_(fsi,
			fat_sector_buff, &temp, fi->cluster, cluster);

	if (S_ISDIR(mode)) {
		/* create . and ..  files of this catalog */
		fat_set_direntry(di->currentcluster, fi->cluster);
		cluster = fi->volinfo->dataarea +
				  ((fi->cluster - 2) * fi->volinfo->secperclus);
		if (fat_write_sector(fsi, fat_sector_buff, cluster)) {
			return DFS_ERRMISC;
		}
	}

	return DFS_OK;
}

void fat_write_longname(char *name, struct fat_dirent *di) {
	struct fat_long_dirent *ld;
	int l;

	assert(name);
	assert(di);

	ld = (void *) di;
	memset(ld->name1, 0xff, sizeof(ld->name1));
	memset(ld->name2, 0xff, sizeof(ld->name2));
	memset(ld->name3, 0xff, sizeof(ld->name3));

	l = strlen(name) + 1;

	for (int i = 0; i < sizeof(ld->name1) / 2; i++) {
		if (i >= l) {
			return;
		}

		ld->name1[i * 2] = name[i];
		ld->name1[i * 2 + 1] = '\0';
	}

	name += sizeof(ld->name1) / 2;
	l -= sizeof(ld->name1) / 2;

	for (int i = 0; i < sizeof(ld->name2) / 2; i++) {
		if (i >= l) {
			return;
		}

		ld->name2[i * 2] = name[i];
		ld->name2[i * 2 + 1] = '\0';
	}

	name += sizeof(ld->name2) / 2;
	l -= sizeof(ld->name2) / 2;

	for (int i = 0; i < sizeof(ld->name3) / 2; i++) {
		if (i >= l) {
			return;
		}

		ld->name3[i * 2] = name[i];
		ld->name3[i * 2 + 1] = '\0';
	}
}

int fat_read_filename(struct fat_file_info *fi, void *p_scratch, char *name) {
	struct fat_fs_info *fsi;
	struct fat_dirent *di;
	int offt = 1;

	assert(name);
	assert(fi);
	assert(fi->fsi);

	fsi = fi->fsi;

	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	name[0] = '\0';

	di = &((struct fat_dirent *) p_scratch)[fi->diroffset];
	if (fi->diroffset > 0) {
		di = &((struct fat_dirent *) p_scratch)[fi->diroffset - 1];
		if (di->attr == ATTR_LONG_NAME) {
			while (di->attr == ATTR_LONG_NAME && fi->diroffset - offt >= 0) {
				fat_append_longname(name, di);
				offt++;
				di = &((struct fat_dirent *) p_scratch)[fi->diroffset - offt];
			}
		} else {
			di = &((struct fat_dirent *) p_scratch)[fi->diroffset];
			strcpy(name, (void *) di->name);
		}
	} else {
		strcpy(name, (void *) di->name);
	}

	/* In FAT names by default are padded with zeroes,
	 * we don't want that in VFS tree */
	offt = strlen(name);
	while (name[offt - 1] == ' ' && offt > 0) {
		name[--offt] = '\0';
	}

	return DFS_OK;
}

static bool fat_old_dirent_symbol(char c) {
	if (c == ' ') {
		return true;
	}

	if (c >= 'A' && c <= 'Z') {
		return true;
	}

	if (c >= '0' && c <= '9') {
		return true;
	}

	return false;
}

/* Count how much long name entries will it take */
int fat_entries_per_name(const char *name) {
	ssize_t l;
	bool old_compat = true;

	assert(name);

	l = strlen(name);

	if (l > MSDOS_NAME) {
		old_compat = false;
	} else {
		for (int i = 0; i < l; i++) {
			if (!fat_old_dirent_symbol(name[i])) {
				old_compat = false;
			}
		}
	}

	if (old_compat) {
		return 1;
	} else {
		int ret = 1; /* We need at least one 8.3 entry */
		while (l > 0) {
			ret++;
			l -= 13;
		}

		return ret;
	}
}

uint32_t fat_get_free_entries(struct fat_fs_info *fsi,
		struct dirinfo *dir, struct fat_dirent *dirent, int n) {
	struct fat_dirent de;
	uint32_t res;
	bool failed;

	assert(fsi);
	assert(dir);
	assert(dirent);

	while (true) {
		failed = false;

		/* Find some free entry */
		while (DFS_OK == (res = fat_get_next(fsi, dir, &de))) {
		}

		if (res != DFS_EOF) {
			break;
		}

		memcpy(dirent, &de, sizeof(de));

		/* Check if following N - 1 entries are free as well */
		for (int i = 0; i < n - 1; i++) {
			res = fat_get_next(fsi, dir, &de);
			if (res != DFS_EOF) {
				failed = true;
				break;
			}
		}

		if (failed) {
			continue;
		}

		return DFS_OK;
	}

	return res;
}

uint32_t fat_write_de(struct fat_fs_info *fsi,
                             struct dirinfo *di, struct fat_dirent *de) {
	uint32_t sector;

	assert(fsi);
	assert(di);
	assert(de);

	sector = di->currentsector + fat_sec_by_clus(fsi, di->currentcluster);

	if (fat_read_sector(fsi, di->p_scratch, sector))
		return -1;

	memcpy(&(((struct fat_dirent *) di->p_scratch)[di->currententry]),
			de, sizeof(struct fat_dirent));
	if (fat_write_sector(fsi, di->p_scratch, sector))
		return -1;

	return 0;
}

uint8_t fat_canonical_name_checksum(const char *name) {
	uint8_t res = 0x00;

	assert(name);

	for (int i = 0; i < MSDOS_NAME; i++) {
		res = (res / 2) + (res % 2) * 0x80;
		res += name[i];
	}

	return res;
}

#include <framework/mod/options.h>

POOL_DEF(fat_fs_pool,
         struct fat_fs_info,
	 OPTION_GET(NUMBER, fat_descriptor_quantity));

POOL_DEF(fat_file_pool,
         struct fat_file_info,
         OPTION_GET(NUMBER, inode_quantity));

POOL_DEF(fat_dirinfo_pool,
         struct dirinfo,
         OPTION_GET(NUMBER, inode_quantity));

POOL_DEF(fat_dirent_pool,
         struct fat_dirent,
         OPTION_GET(NUMBER, inode_quantity));

struct fat_fs_info *fat_fs_alloc(void) {
	return pool_alloc(&fat_fs_pool);
}

void fat_fs_free(struct fat_fs_info *fsi) {
	pool_free(&fat_fs_pool, fsi);
}

struct fat_file_info *fat_file_alloc(void) {
	return pool_alloc(&fat_file_pool);
}

void fat_file_free(struct fat_file_info *fi) {
	pool_free(&fat_file_pool, fi);
}

struct dirinfo *fat_dirinfo_alloc(void) {
	return pool_alloc(&fat_dirinfo_pool);
}

void fat_dirinfo_free(struct dirinfo *di) {
	pool_free(&fat_dirinfo_pool, di);
}
