/**
 * @file   fat_common.c
 * @brief  VFS-independent part of FAT driver
 * @data   9 Apr 2015
 * @author Denis Deryugin
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <framework/mod/options.h>
#include <fs/fat.h>
#include <mem/misc/pool.h>
#include <util/math.h>

#define LABEL "EMBOX_DISK"
#define SYSTEM "FAT12"

extern uint8_t fat_sector_buff[512]; /* XXX */

uint32_t fat_get_next(struct fat_fs_info *fsi,
		struct dirinfo * dirinfo, struct dirent * dirent);

char bootcode[130] =
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

int fat_read_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector) {
	assert(fsi->bdev);
	assert(fsi->vi.bytepersec);

	int dev_blk_size = fsi->bdev->driver->ioctl(fsi->bdev, IOCTL_GETBLKSIZE, NULL, 0);
	assert(dev_blk_size > 0);
	int sec_size = fsi->vi.bytepersec;

	if (0 > block_dev_read(fsi->bdev, (char *) buffer, sec_size, sector * sec_size / dev_blk_size)) {
		return DFS_ERRMISC;
	} else {
		return DFS_OK;
	}
}

int fat_write_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector) {
	assert(fsi->bdev);
	assert(fsi->vi.bytepersec);

	int dev_blk_size = fsi->bdev->driver->ioctl(fsi->bdev, IOCTL_GETBLKSIZE, NULL, 0);
	assert(dev_blk_size > 0);
	int sec_size = fsi->vi.bytepersec;

	if (0 > block_dev_write(fsi->bdev, (char *) buffer, sec_size, sector * sec_size / dev_blk_size)) {
		return DFS_ERRMISC;
	} else {
		return DFS_OK;
	}
}

int fat_create_partition(void *bdev) {
	uint16_t bytepersec = 512;
	size_t num_sect = block_dev(bdev)->size / bytepersec;
	uint16_t secperfat = max(1, (uint16_t) 0xFFFF & (num_sect / bytepersec ));
	uint16_t rootentries = 0x0200; /* 512 for FAT16 */

	struct lbr lbr = {
		.jump = {0xeb, 0x3c, 0x90}, /* JMP 0x3c; NOP; */
		.oemid = {0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45}, /* ASCII "EEEEEEEE" */
		.bpb = {
			.bytepersec_l = (uint8_t)(bytepersec & 0xFF),
			.bytepersec_h = (uint8_t)((bytepersec & 0xFF00) >> 8),
			.secperclus = 0x04,
			.reserved_l = 0x01,	/* reserved sectors */
			.reserved_h = 0x0,
			.numfats = 0x02,	/* 2 FAT copy */
			.mediatype = 0xF8,
			.secpertrk_l = 0x3F,
			.heads_l = 0xFF,
			.secperfat_l = (uint8_t)(0x00FF & secperfat),
			.secperfat_h = (uint8_t)(0x00FF & (secperfat >> 8)),
			.rootentries_l = (uint8_t)(0x00FF & rootentries ),
			.rootentries_h = (uint8_t)(0x00FF & (rootentries >> 8)),
		},
		.ebpb = {
			.ebpb = {
				.signature = 0x29,
				.serial_0 = 0x81,
				.serial_1 = 0xDB,
				.serial_2 = 0xF7,
				.serial_3 = 0xBB,
				.label = LABEL,
				.system = SYSTEM,
			},
		},
		.sig_55 = 0x55,
		.sig_aa = 0xAA,
	};

	memcpy(lbr.ebpb.ebpb.system + 8, bootcode, 130);

	if (0xFFFF > num_sect)	{
		lbr.bpb.sectors_s_l = (uint8_t)(0x00000FF & num_sect);
		lbr.bpb.sectors_s_h = (uint8_t)(0x00000FF & (num_sect >> 8));
	} else {
		lbr.bpb.sectors_l_0 = (uint8_t)(0x00000FF & num_sect );
		lbr.bpb.sectors_l_1 = (uint8_t)(0x00000FF & (num_sect >> 8));
		lbr.bpb.sectors_l_2 = (uint8_t)(0x00000FF & (num_sect >> 16));
		lbr.bpb.sectors_l_3 = (uint8_t)(0x00000FF & (num_sect >> 24));
	}

	return 0 < block_dev_write(bdev, (void *) &lbr, sizeof(lbr), 0);
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

	volinfo->bytepersec = lbr->bpb.bytepersec_l + (lbr->bpb.bytepersec_h << 8);
	volinfo->startsector = startsector;

	if (0 > block_dev_read(	bdev,
				(char *) fat_sector_buff,
				sizeof(struct lbr),
				0)) { /* TODO start sector */
		return DFS_ERRMISC;
	}

	volinfo->secperclus = lbr->bpb.secperclus;
	volinfo->reservedsecs = (uint16_t) lbr->bpb.reserved_l |
		  (((uint16_t) lbr->bpb.reserved_h) << 8);

	volinfo->numsecs =  (uint16_t) lbr->bpb.sectors_s_l |
		  (((uint16_t) lbr->bpb.sectors_s_h) << 8);

	if (!volinfo->numsecs) {
		volinfo->numsecs = (uint32_t) lbr->bpb.sectors_l_0 |
		  (((uint32_t) lbr->bpb.sectors_l_1) << 8) |
		  (((uint32_t) lbr->bpb.sectors_l_2) << 16) |
		  (((uint32_t) lbr->bpb.sectors_l_3) << 24);
	}

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

	}
	else {
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
		volinfo->dataarea = volinfo->rootdir + (((volinfo->rootentries * 32) +
				(volinfo->bytepersec - 1)) / volinfo->bytepersec);
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
		volinfo->numclusters =
				(volinfo->numsecs - volinfo->dataarea) / volinfo->secperclus;
	}

	if (volinfo->numclusters < 4085) {
		volinfo->filesystem = FAT12;
	} else if (volinfo->numclusters < 65525) {
		volinfo->filesystem = FAT16;
	} else {
		volinfo->filesystem = FAT32;
	}

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
			return 0x0ffffff7;	/* FAT32 bad cluster */
	}
	/*
	 * at this point, offset is the BYTE offset of the desired sector from
	 * the start of the FAT. Calculate the physical sector containing this
	 *  FAT entry.
	 */
	sector = ldiv(offset, volinfo->bytepersec).quot + volinfo->fat1;

	if (sector != *p_scratchcache) {
		if (fat_read_sector(fsi, p_scratch, sector)) {
			/*
			 * avoid anyone assuming that this cache value is still valid,
			 * which might cause disk corruption
			 */
			*p_scratchcache = 0;
			return 0x0ffffff7;	/* FAT32 bad cluster */
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
	offset = ldiv(offset, volinfo->bytepersec).rem;
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
				return 0x0ffffff7;	/* FAT32 bad cluster */
			}
			*p_scratchcache = sector;
			result |= ((uint32_t) p_scratch[0]) << 8;
		} else {
			result = (uint32_t) p_scratch[offset] |
			  ((uint32_t) p_scratch[offset+1]) << 8;
		}
		if (cluster & 1) {
			result = result >> 4;
		} else {
			result = result & 0xfff;
		}
	} else if (volinfo->filesystem == FAT16) {
		result = (uint32_t) p_scratch[offset] |
		  ((uint32_t) p_scratch[offset+1]) << 8;
	} else if (volinfo->filesystem == FAT32) {
		result = ((uint32_t) p_scratch[offset] |
		  ((uint32_t) p_scratch[offset+1]) << 8 |
		  ((uint32_t) p_scratch[offset+2]) << 16 |
		  ((uint32_t) p_scratch[offset+3]) << 24) & 0x0fffffff;
	} else {
		result = 0x0ffffff7;	/* FAT32 bad cluster */
	}
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
	sector = ldiv(offset, volinfo->bytepersec).quot + volinfo->fat1;

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
	offset = ldiv(offset, volinfo->bytepersec).rem;

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
		if (DFS_OK == result) {
			result = fat_write_sector(fsi, p_scratch,
					(*p_scratchcache) + volinfo->secperfat);
		}
		break;
	default:
		result = DFS_ERRMISC;
	}
	return result;
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
		if (!result) {
			return i;
		}
	}
	return 0x0ffffff7;
}

static inline int dir_is_root(uint8_t *name) {
	return (!strlen((char *) name)) ||
		(((strlen((char *) name) == 1) && (name[0] == DIR_SEPARATOR)));
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

	volinfo = &fsi->vi;
	dirinfo->flags = 0;
	dirinfo->currentsector = 0;
	dirinfo->currententry = 0;

	if (dir_is_root(dirname)) {
		if (volinfo->filesystem == FAT32) {
			dirinfo->currentcluster = volinfo->rootdir;
			return fat_read_sector(fsi, dirinfo->p_scratch,
					volinfo->dataarea +	((volinfo->rootdir - 2)
							* volinfo->secperclus));
		} else {
			dirinfo->currentcluster = 0;
			return fat_read_sector(fsi, dirinfo->p_scratch,
					volinfo->rootdir);
		}
	} else {
		uint8_t tmpfn[12];
		uint8_t *ptr = dirname;
		uint32_t result;
		struct dirent de;

		if (volinfo->filesystem == FAT32) {
			dirinfo->currentcluster = volinfo->rootdir;
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

			de.name[0] = 0;

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
						((dirinfo->currentcluster - 2) *
								volinfo->secperclus))) {
					return DFS_ERRMISC;
				}
			} else if (!memcmp(de.name, tmpfn, MSDOS_NAME) && !(de.attr & ATTR_DIRECTORY)) {
				return DFS_NOTFOUND;
			}

			while (*ptr != DIR_SEPARATOR && *ptr) {
				ptr++;
			}
			if (*ptr == DIR_SEPARATOR) {
				ptr++;
			}
		}

		if (!dirinfo->currentcluster) {
			return DFS_NOTFOUND;
		}
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
		struct dirinfo *dirinfo, struct dirent *dirent) {
	struct volinfo *volinfo;
	uint32_t tempint;
	volinfo = &fsi->vi;

	/* Do we need to read the next sector of the directory? */
	if (dirinfo->currententry >= volinfo->bytepersec / sizeof(struct dirent)) {
		dirinfo->currententry = 0;
		dirinfo->currentsector++;

		/* Root directory; special case handling
		 * Note that currentcluster will only ever be zero if both:
		 * (a) this is the root directory, and
		 * (b) we are on a FAT12/16 volume, where the root dir can't be
		 * expanded
		 */
		if (dirinfo->currentcluster == 0) {
			/* Trying to read past end of root directory? */
			if (dirinfo->currentsector * (volinfo->bytepersec / sizeof(struct dirent)) >=
					volinfo->rootentries) {
				return DFS_EOF;
			}

			/* Otherwise try to read the next sector */
			if (fat_read_sector(fsi, dirinfo->p_scratch,
					volinfo->rootdir + dirinfo->currentsector)) {
				return DFS_ERRMISC;
			}
		} else {
			if (dirinfo->currentsector >= volinfo->secperclus) {
				dirinfo->currentsector = 0;
				if ((dirinfo->currentcluster >= 0xff7 &&
						volinfo->filesystem == FAT12) ||
						(dirinfo->currentcluster >= 0xfff7 &&
						volinfo->filesystem == FAT16) ||
						(dirinfo->currentcluster >= 0x0ffffff7 &&
						volinfo->filesystem == FAT32)) {

				  	/* We are at the end of the directory chain.
				  	 * If this is a normal find operation, we should indicate
				  	 * that there is nothing more to see.
				  	 */
				  	if (!(dirinfo->flags & DFS_DI_BLANKENT)) {
				  		return DFS_EOF;
				  	}

					/*
					 * On the other hand, if this is a "find free entry"
					 * search, we need to tell the caller to allocate a
					 * new cluster
					 */
					else {
						return DFS_ALLOCNEW;
					}
				}
				//assert(nas->fs->bdev == fsi->bdev);
				dirinfo->currentcluster = fat_get_fat_(fsi,
						dirinfo->p_scratch, &tempint, dirinfo->currentcluster);
			}
			if (fat_read_sector(fsi, dirinfo->p_scratch,
					volinfo->dataarea +	((dirinfo->currentcluster - 2)
						* volinfo->secperclus) + dirinfo->currentsector)) {
				return DFS_ERRMISC;
			}
		}
	}

	memcpy(dirent, &(((struct dirent*) dirinfo->p_scratch)[dirinfo->currententry]),
			sizeof(struct dirent));

	if (dirent->name[0] == 0) {
		if (dirinfo->flags & DFS_DI_BLANKENT) {
			dirinfo->currententry++; // DOSFS 1.03 BUG, currententry was not incremented in this case
			return DFS_OK;
		}
		else {
			return DFS_EOF;
		}
	}

	if (dirent->name[0] == 0xe5 || (dirent->attr & ATTR_LONG_NAME) == ATTR_LONG_NAME) {
		dirent->name[0] = 0;
	} else if (dirent->name[0] == 0x05) {
		/* handle kanji filenames beginning with 0xE5*/
		dirent->name[0] = 0xe5;
	}

	dirinfo->currententry++;

	return DFS_OK;
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
		struct dirinfo *di, struct dirent *de) {
	uint32_t tempclus,i;
	struct volinfo *volinfo;

	volinfo = &fsi->vi;

	if (fat_open_dir(fsi, path, di)) {
		return DFS_NOTFOUND;
	}

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
			//assert(nas->fs->bdev == fsi->bdev);
			tempclus = fat_get_free_fat_(fsi, di->p_scratch);
			if (tempclus == 0x0ffffff7) {
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
			//assert(nas->fs->bdev == fsi->bdev);
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

#define MSDOS_DOT     ".          "
#define MSDOS_DOTDOT  "..         "
void fat_set_direntry (uint32_t dir_cluster, uint32_t cluster) {
	struct dirent *de = (struct dirent *) fat_sector_buff;

	de[0] = (struct dirent) {
		.name = MSDOS_DOT,
		.attr = ATTR_DIRECTORY,
		.startclus_l_l = cluster & 0xff,
		.startclus_l_h = (cluster & 0xff00) >> 8,
		.startclus_h_l = (cluster & 0xff0000) >> 16,
		.startclus_h_h = (cluster & 0xff000000) >> 24,
	};

	de[1] = (struct dirent) {
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
	struct dirent de;
	int dev_blk_size = ((struct block_dev*) bdev)->driver->ioctl(bdev, IOCTL_GETBLKSIZE, 0, 0);

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

	cluster = fat_get_free_fat_(&fsi, fat_sector_buff);

	de = (struct dirent) {
		.name = "/ROOT      ",
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

	if (0 > block_dev_read(	bdev,
				(char *) fat_sector_buff,
				sizeof(struct dirent),
				fsi.vi.rootdir * fsi.vi.bytepersec / dev_blk_size)) {
		return DFS_ERRMISC;
	}
	/* we clear other FAT TABLE */
	memset(fat_sector_buff, 0, sizeof(fat_sector_buff));

	memcpy(&(((struct dirent*) fat_sector_buff)[0]), &de, sizeof(struct dirent));

	if (0 > block_dev_write(	bdev,
					(char *) fat_sector_buff,
					sizeof(struct dirent),
					fsi.vi.rootdir * fsi.vi.bytepersec / dev_blk_size)) {
		return DFS_ERRMISC;
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
				//assert(nas->fs->bdev == fsi->bdev);
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

	if (!(fi->mode & O_WRONLY) && !(fi->mode & O_APPEND)) {
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
			//assert(nas->fs->bdev == fsi->bdev);
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
				//assert(nas->fs->bdev == fsi->bdev);
				tempclus = fat_get_free_fat_(fsi, p_scratch);
				byteswritten = 0; /* invalidate cache */
				if (tempclus == 0x0ffffff7) {
					return DFS_ERRMISC;
				}
				/* Link new cluster onto file */
				//assert(nas->fs->bdev == fsi->bdev);
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
	if (*size > fi->pointer) {
		if (div(*size, clastersize).quot !=
			div(fi->pointer, clastersize).quot) {

			byteswritten = 0;/* invalidate cache */
			//assert(nas->fs->bdev == fsi->bdev);
			nextcluster = fat_get_fat_(fsi, p_scratch,
					&byteswritten, fi->cluster);

			switch(fi->volinfo->filesystem) {
				case FAT12:		lastcluster = 0xfff;	break;
				case FAT16:		lastcluster = 0xfff;	break;
				case FAT32:		lastcluster = 0x0fffffff;	break;
				default:		return DFS_ERRMISC;
			}
			//assert(nas->fs->bdev == fsi->bdev);
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
				//assert(nas->fs->bdev == fsi->bdev);
				nextcluster = fat_get_fat_(fsi, p_scratch,
						&byteswritten, nextcluster);

				fat_set_fat_(fsi, p_scratch,
						&byteswritten, lastcluster, 0);
			}

		}
	}

	*size = fi->pointer;

	/* Update directory entry */
	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	((struct dirent*) p_scratch)[fi->diroffset].filesize_0 =
			 *size & 0xff;

	((struct dirent*) p_scratch)[fi->diroffset].filesize_1 =
			(*size & 0xff00) >> 8;

	((struct dirent*) p_scratch)[fi->diroffset].filesize_2 =
			(*size & 0xff0000) >> 16;

	((struct dirent*) p_scratch)[fi->diroffset].filesize_3 =
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
	uint8_t filename[12];
	struct dirinfo di;
	struct dirent de;

	struct volinfo *volinfo;
	struct fat_fs_info *fsi;

	fsi = fi->fsi;
	volinfo = &fsi->vi;

	fi->mode = mode;

	strncpy((char *) tmppath, (char *) path, PATH_MAX);
	tmppath[PATH_MAX - 1] = 0;
	if (strcmp((char *) path,(char *) tmppath)) {
		return DFS_PATHLEN;
	}

	fat_get_filename(tmppath, (char *) filename);

	/*
	 *  At this point, if our path was MYDIR/MYDIR2/FILE.EXT,
	 *  filename = "FILE    EXT" and  tmppath = "MYDIR/MYDIR2".
	 */

	di.p_scratch = p_scratch;
	if (fat_open_dir(fsi, (uint8_t *) tmppath, &di)) {
		return DFS_NOTFOUND;
	}

	while (!fat_get_next(fsi, &di, &de)) {
		path_canonical_to_dir(tmppath, (char *) de.name);
		if (!memcmp(tmppath, filename, MSDOS_NAME)) {
			if (de.attr & ATTR_DIRECTORY){
				return DFS_NOTFOUND;
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
			if (size)
				*size = (uint32_t) de.filesize_0 |
				  ((uint32_t) de.filesize_1) << 8 |
				  ((uint32_t) de.filesize_2) << 16 |
				  ((uint32_t) de.filesize_3) << 24;

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

	if (DFS_OK != fat_open_file(fi, path, O_RDONLY, p_scratch, NULL)) {
		return DFS_NOTFOUND;
	}

	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	((struct dirent*) p_scratch)[fi->diroffset].name[0] = 0xe5;
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
	if (DFS_OK != fat_open_file(fi, path, O_RDONLY, p_scratch, NULL)) {
		return DFS_NOTFOUND;
	}

	/* First, read the directory sector and delete that entry */
	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	((struct dirent*) p_scratch)[fi->diroffset].name[0] = 0xe5;
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

/*
 * Create a file or directory. You supply a file_create_param_t
 * structure.
 * Returns various DFS_* error states. If the result is DFS_OK, file
 * was created and can be used.
 */
int fat_create_file(struct fat_file_info *fi, char *path, int mode) {
	uint8_t filename[12];
	struct dirinfo di;
	struct dirent de;
	struct volinfo *volinfo;
	uint32_t cluster, temp;
	struct fat_fs_info *fsi;

	fsi = fi->fsi;
	volinfo = &fsi->vi;
	fi->volinfo = volinfo;

	fat_get_filename(path, (char *) filename);

	/*
	 *  At this point, if our path was MYDIR/MYDIR2/FILE.EXT,
	 *  filename = "FILE    EXT" and  path = "MYDIR/MYDIR2".
	 */
	di.p_scratch = fat_sector_buff;
	if (fat_open_dir(fsi, (uint8_t *) path, &di)) {
		return DFS_NOTFOUND;
	}

	while (!fat_get_next(fsi, &di, &de));

	/* Locate or create a directory entry for this file */
	if (DFS_OK != fat_get_free_dir_ent(fsi, (uint8_t *) path, &di, &de)) {
		return DFS_ERRMISC;
	}

	//assert(nas->fs->bdev == fsi->bdev);
	cluster = fat_get_free_fat_(fsi, fat_sector_buff);
	de = (struct dirent) {
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
	if (di.currentcluster == 0) {
		fi->dirsector = volinfo->rootdir + di.currentsector;
	} else {
		fi->dirsector = volinfo->dataarea +
				((di.currentcluster - 2) * volinfo->secperclus) +
				di.currentsector;
	}
	fi->diroffset = di.currententry - 1;
	fi->cluster = cluster;
	fi->firstcluster = cluster;
	//nas->fi->ni.size = 0;

	/*
	 * write the directory entry
	 * note that we no longer have the sector containing the directory
	 * entry, tragically, so we have to re-read it
	 */

	if (fat_read_sector(fsi, fat_sector_buff, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	memcpy(&(((struct dirent*) fat_sector_buff)[di.currententry - 1]),
			&de, sizeof(struct dirent));
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
	//assert(nas->fs->bdev == fsi->bdev);
	fat_set_fat_(fsi,
			fat_sector_buff, &temp, fi->cluster, cluster);

	if (S_ISDIR(mode)) {
		/* create . and ..  files of this catalog */
		fat_set_direntry(di.currentcluster, fi->cluster);
		cluster = fi->volinfo->dataarea +
				  ((fi->cluster - 2) * fi->volinfo->secperclus);
		if (fat_write_sector(fsi, fat_sector_buff, cluster)) {
			return DFS_ERRMISC;
		}
	}

	return DFS_OK;
}

/* fat filesystem description pool */
POOL_DEF(fat_fs_pool, struct fat_fs_info, 4);
	//OPTION_GET(NUMBER, fat_descriptor_quantity));

/* fat file description pool */
POOL_DEF(fat_file_pool, struct fat_file_info, 16);
	//OPTION_GET(NUMBER, inode_quantity));

struct fat_fs_info *fat_fs_pool_alloc(void) {
	return pool_alloc(&fat_fs_pool);
}

void fat_fs_pool_free(struct fat_fs_info *fsi) {
	pool_free(&fat_fs_pool, fsi);
}

struct fat_file_info *fat_file_pool_alloc(void) {
	return pool_alloc(&fat_file_pool);
}

void fat_file_pool_free(struct fat_file_info *fi) {
	pool_free(&fat_file_pool, fi);
}

