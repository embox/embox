/**
 * @file
 * @brief  VFS-independent part of FAT driver
 * @data   9 Apr 2015
 * @author Denis Deryugin
 */
#include <util/log.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <drivers/block_dev.h>
#include <fs/dir_context.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/mount.h>
#include <fs/super_block.h>

#include <fs/mbr.h>

#include "fat.h"

#include <util/log.h>
#include <util/math.h>

#include <framework/mod/options.h>

#define FAT_USE_LONG_NAMES OPTION_GET(BOOLEAN, fat_max_sector_size)

#if OPTION_GET(STRING, log_level) >= 4
void fat_volinfo_print(struct volinfo *volinfo) {
	log_debug("volinfo->label(%s)", volinfo->label);
	log_debug("volinfo->unit(%x)", volinfo->unit);
	log_debug("volinfo->filesystem(%x)", volinfo->filesystem);
	log_debug("volinfo->startsector(%d)", volinfo->startsector);
	log_debug("volinfo->bytepersec(%d)", volinfo->bytepersec);
	log_debug("volinfo->secperclus(%d)", volinfo->secperclus);
	log_debug("volinfo->reservedsecs(%d)", volinfo->reservedsecs);
	log_debug("volinfo->numsecs(%d)", volinfo->numsecs);
	log_debug("volinfo->secperfat(%d)", volinfo->secperfat);
	log_debug("volinfo->rootentries(%d)", volinfo->rootentries);
	log_debug("volinfo->numclusters(%d)", volinfo->numclusters);
	log_debug("volinfo->fat1(%d)", volinfo->fat1);
	log_debug("volinfo->rootdir(%d)", volinfo->rootdir);
	log_debug("volinfo->dataarea(%d)", volinfo->dataarea);
}
#else
#define fat_volinfo_print(volinfo)
#endif

#define LABEL    "EMBOX_DISK " /* Whitespace-padded 11-char string */
#define SYSTEM12 "FAT12   "
#define SYSTEM16 "FAT16   "
#define SYSTEM32 "FAT32   "

uint8_t fat_sector_buff[FAT_MAX_SECTOR_SIZE] __attribute__((aligned(16)));

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

static uint32_t fat_dir_rewind(struct dirinfo *di, int n);
static uint32_t fat_write_de(struct dirinfo *di, struct fat_dirent *de);
static uint32_t fat_get_free_entries(struct dirinfo *dir, int n);
static uint32_t fat_dir_extend(struct dirinfo *di);
int fat_read_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector) {
	size_t ret;
	int blk;
	int blkpersec = fsi->vi.bytepersec / fsi->bdev->block_size;

	log_debug("sector(%d), fsi: bytepersec(%d), bdev->block_size(%d)",
			sector, fsi->vi.bytepersec, fsi->bdev->block_size);
	blk = sector * blkpersec;

	ret = block_dev_read(fsi->bdev, (char*) buffer, fsi->vi.bytepersec, blk);
	if (ret != fsi->vi.bytepersec)
		return DFS_ERRMISC;
	else
		return DFS_OK;
}

int fat_write_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector) {
	size_t ret;
	int blk;
	int blkpersec = fsi->vi.bytepersec / fsi->bdev->block_size;

	log_debug("sector(%d), fsi: bytepersec(%d), bdev->block_size(%d)",
			sector, fsi->vi.bytepersec, fsi->bdev->block_size);

	blk = sector * blkpersec;
	ret = block_dev_write(fsi->bdev, (char*) buffer, fsi->vi.bytepersec, blk);
	if (ret != fsi->vi.bytepersec)
		return DFS_ERRMISC;
	else
		return DFS_OK;
}

uint32_t fat_current_dirsector(struct dirinfo *di) {
	struct fat_fs_info *fsi = di->fi.fsi;
	struct volinfo *vi = &fsi->vi;

	if (di->fi.dirsector == 0 &&
			(vi->filesystem == FAT12 || vi->filesystem == FAT16)) {
		return fsi->vi.secperclus * di->currentcluster + di->currentsector;
	} else {
		return fat_sec_by_clus(fsi, di->currentcluster) + di->currentsector;
	}
}
/**
* @brief Read related dir entries into dir buffer
*
* @param fsi Used to determine bdev and fat type (12/16/32)
* @param di  Pointer to dirinfo structure
*
* @return Negative error number
* @retval 0 Success
*/
int read_dir_buf(struct dirinfo *di) {
	struct fat_fs_info *fsi = di->fi.fsi;
	int sector;

	sector = fat_current_dirsector(di);

	return fat_read_sector(fsi, di->p_scratch, sector);
}

uint32_t fat_direntry_get_clus(struct fat_dirent *de) {
	return (uint32_t) de->startclus_l_l |
	  ((uint32_t) de->startclus_l_h) << 8 |
	  ((uint32_t) de->startclus_h_l) << 16 |
	  ((uint32_t) de->startclus_h_h) << 24;
}

void fat_direntry_set_clus(struct fat_dirent *de, uint32_t clus) {
	de->startclus_l_l = clus & 0xff;
	de->startclus_l_h = (clus & 0xff00) >> 8;
	de->startclus_h_l = (clus & 0xff0000) >> 16;
	de->startclus_h_h = (clus & 0xff000000) >> 24;
}

uint32_t fat_direntry_get_size(struct fat_dirent *de) {
	return  (uint32_t) de->filesize_0 |
		((uint32_t) de->filesize_1) << 8 |
		((uint32_t) de->filesize_2) << 16 |
		((uint32_t) de->filesize_3) << 24;
}

void fat_direntry_set_size(struct fat_dirent *de, uint32_t size) {
	de->filesize_0 = size & 0xff;
	de->filesize_1 = (size & 0xff00) >> 8;
	de->filesize_2 = (size & 0xff0000) >> 16;
	de->filesize_3 = (size & 0xff000000) >> 24;
}
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

	log_debug("startsector %d", startsector);

	if (0 > block_dev_read(bdev,
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
		fat_volinfo_print(volinfo);
		return DFS_ERRMISC;
	} else {
		volinfo->numclusters = (volinfo->numsecs - volinfo->dataarea) /
			volinfo->secperclus;
	}

	if (volinfo->numclusters < 4085) {
		volinfo->filesystem = FAT12;
	} else if (volinfo->numclusters < 65525) {
		volinfo->filesystem = FAT16;
	} else {
		volinfo->filesystem = FAT32;
	}

	fat_volinfo_print(volinfo);

	return DFS_OK;
}

/*
 *	Fetch FAT entry for specified cluster number You must provide a scratch
 *	buffer for one sector (SECTOR_SIZE) and a populated struct volinfo
 *	Returns a FAT32 BAD_CLUSTER value for any error, otherwise the contents
 *	of the desired FAT entry.
 */
uint32_t fat_get_fat(struct fat_fs_info *fsi,
		uint8_t *p_scratch, uint32_t cluster) {
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

	if (fat_read_sector(fsi, p_scratch, sector)) {
		return DFS_BAD_CLUS;
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
				return DFS_BAD_CLUS;
			}
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

static uint32_t fat_end_of_chain(struct fat_fs_info *fsi) {
	switch(fsi->vi.filesystem) {
	case FAT12:
		return 0xfff;
	case FAT16:
		return 0xffff;
	case FAT32:
		return 0x0fffffff; /* FAT32 is really "FAT28" */
	default:
		return 0;
	}
}

static uint32_t fat_is_end_of_chain(struct fat_fs_info *fsi, uint32_t clus) {
	switch (fsi->vi.filesystem) {
	case FAT12:
		return clus >= 0xff7;
	case FAT16:
		return clus >= 0xfff7;
	case FAT32:
		return clus >= 0xffffff7;
	}

	return 1;
}

/*
 * Set FAT entry for specified cluster number
 * You must provide a scratch buffer for one sector (SECTOR_SIZE)
 * and a populated volinfo_t Returns DFS_ERRMISC for any error, otherwise
 * DFS_OK p_scratchcache should point to a UINT32.
 * */
static uint32_t fat_set_fat(struct fat_fs_info *fsi, uint8_t *p_scratch,
		uint32_t cluster, uint32_t new_contents) {
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
		return DFS_ERRMISC;
	}

	new_contents &= fat_end_of_chain(fsi);
	/*
	 * at this point, offset is the BYTE offset of the desired sector from
	 * the start of the FAT.
	 * Calculate the physical sector containing this FAT entry.
	 */
	sector = offset / volinfo->bytepersec + volinfo->fat1;

	if (fat_read_sector(fsi, p_scratch, sector)) {
		return DFS_ERRMISC;
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
			result = fat_write_sector(fsi, p_scratch, sector);
			/* mirror the FAT into copy 2 */
			if (DFS_OK == result) {
				result = fat_write_sector(fsi, p_scratch,
						sector + volinfo->secperfat);
			}

			/*
			 * If we wrote that sector OK, then read in the subsequent sector
			 * and poke the first byte with the remainder of this FAT entry.
			 */
			if (DFS_OK == result) {
				result = fat_read_sector(fsi, p_scratch, ++sector);
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
					result = fat_write_sector(fsi, p_scratch, sector);
					/* mirror the FAT into copy 2 */
					if (DFS_OK == result) {
						result = fat_write_sector(fsi, p_scratch,
								sector+volinfo->secperfat);
					}
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
			result = fat_write_sector(fsi, p_scratch, sector);
			/* mirror the FAT into copy 2 */
			if (DFS_OK == result) {
				result = fat_write_sector(fsi, p_scratch,
						sector + volinfo->secperfat);
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
		result = fat_write_sector(fsi, p_scratch, sector);
		/* mirror the FAT into copy 2 */
		if (DFS_OK == result)
			result = fat_write_sector(
					fsi,
					p_scratch,
					sector + volinfo->secperfat
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
static uint32_t fat_get_free_fat(struct fat_fs_info *fsi, uint8_t *p_scratch) {
	uint32_t i;
	/*
	 * Search starts at cluster 2, which is the first usable cluster
	 * NOTE: This search can't terminate at a bad cluster, because there might
	 * legitimately be bad clusters on the disk.
	 */
	for (i = 2; i < fsi->vi.numclusters; i++) {
		if (!fat_get_fat(fsi, p_scratch, i)) {
			return i;
		}
	}
	return DFS_BAD_CLUS;
}

uint32_t fat_open_rootdir(struct fat_fs_info *fsi, struct dirinfo *dirinfo) {
	struct volinfo *volinfo;
	uint32_t ret;

	assert(fsi);

	volinfo = &fsi->vi;

	dirinfo->flags = 0;
	dirinfo->fi.mode = S_IFDIR;
	dirinfo->currentsector = volinfo->rootdir % volinfo->secperclus;
	dirinfo->currentcluster = volinfo->rootdir / volinfo->secperclus;

	log_debug("dirinfo: p_scratch(%p), currentsector(%d), currentcluster(%d)",
			dirinfo->p_scratch, dirinfo->currentsector, dirinfo->currentcluster);
	log_debug("volinfo: secperclus %d rootdir %d", volinfo->secperclus, volinfo->rootdir);

	ret = fat_read_sector(fsi, dirinfo->p_scratch, volinfo->rootdir);

	return ret;
}

static uint32_t fat_fetch_dir(struct dirinfo *dir) {
	struct fat_fs_info *fsi = dir->fi.fsi;
	struct volinfo *volinfo = &fsi->vi;
	int ent_per_sec = volinfo->bytepersec / sizeof(struct fat_dirent);
	int read_sector;

	if (dir->currententry >= ent_per_sec) {
		int next_ent;
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
				int tempclus;

				dir->currentsector = 0;

				tempclus = fat_get_fat(fsi,
						dir->p_scratch,
						dir->currentcluster);

				if (fat_is_end_of_chain(fsi, tempclus)) {
					return DFS_ALLOCNEW;
				} else {
					dir->currentcluster = tempclus;
				}
			}

			read_sector = fat_current_dirsector(dir);

			if (fat_read_sector(fsi, dir->p_scratch, read_sector))
				return DFS_ERRMISC;
		}
	}

	return DFS_OK;
}

static uint32_t fat_get_current(struct dirinfo *dir, struct fat_dirent *dirent) {
	struct fat_dirent *dirent_src;
	uint32_t tmp;

	/* Do we need to read the next sector of the directory? */
	if (DFS_OK != (tmp = fat_fetch_dir(dir))) {
		return tmp;
	}

	dirent_src = &((struct fat_dirent *) dir->p_scratch)[dir->currententry];
	memcpy(dirent, dirent_src, sizeof(struct fat_dirent));

	if (dirent->name[0] == '\0') {
		return DFS_EOF;
	}

	if (dirent->name[0] == 0xe5) {
		dirent->name[0] = '\0';
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
uint32_t fat_get_next(struct dirinfo *dir, struct fat_dirent *dirent) {
	struct fat_dirent *dirent_src;
	uint32_t tmp;

	dirent->name[0] = '\0';

	log_debug("dir->currententry %d", dir->currententry);
	/* Do we need to read the next sector of the directory? */
	if (DFS_OK != (tmp = fat_fetch_dir(dir))) {
		if (tmp == DFS_ALLOCNEW) {
			if (DFS_OK != (tmp = fat_dir_extend(dir))) {
				return tmp;
			}
		} else {
			return tmp;
		}
	}

	dirent_src = &((struct fat_dirent *) dir->p_scratch)[dir->currententry];
	memcpy(dirent, dirent_src, sizeof(struct fat_dirent));
	log_debug("dir: p_scratch = %p, currententry = %d",dir->p_scratch, dir->currententry);
	log_debug("dirent->name (%s), dirent->name[0]=0x%x", dirent->name, dirent->name[0]);

	if (dirent->name[0] == '\0') {
		if (dir->flags & DFS_DI_BLANKENT) {
			dir->currententry++; // DOSFS 1.03 BUG, currententry was not incremented in this case
			return DFS_OK;
		} else {
			return DFS_EOF;
		}
	}

	if (dirent->name[0] == 0xe5) {
		memset(dirent, 0, sizeof(*dirent));
	} else if (dirent->name[0] == 0x05)
		/* handle kanji filenames beginning with 0xE5 */
		dirent->name[0] = 0xe5;

	dir->currententry++;

	return DFS_OK;
}

/* Same as fat_get_next(), but skip long-name entries with following 8.3-entries */
uint32_t fat_get_next_long(struct dirinfo *dir, struct fat_dirent *dirent, char *name_buf) {
	uint32_t ret;
	char c;
	int i;

	assert(dir);
	assert(dir->p_scratch);
	assert(dirent);
	assert(dir->fi.fsi);

	do {
		ret = fat_get_next(dir, dirent);
		if (ret == DFS_EOF) {
			return ret;
		}
	} while (dirent->name[0] == '\0');

	if (dirent->attr != ATTR_LONG_NAME) {
		if (name_buf != NULL) {
			/* Copy name body */
			for (i = 0; i < 8; i++) {
				c = dirent->name[i];
				if (c == ' ') {
					break;
				}

				if (isupper(c)) {
					c += 0x20;
				}

				*name_buf++ = c;
			}

			/* Copy name extension */
			if (dirent->name[8] != ' ') {
				*name_buf++ = '.';
				for (i = 8; i < 11; i++) {
					c = dirent->name[i];
					if (c == ' ')
						break;

					if (isupper(c)) {
						c += 0x20;
					}

					*name_buf++ = c;
				}
			}

			/* Terminate SFN str by a \0 */
			*name_buf = 0;
		}
	} else {
		while (dirent->attr == ATTR_LONG_NAME) {
			if (name_buf != NULL) {
				fat_append_longname(name_buf, dirent);
			}
			ret = fat_get_next(dir, dirent);
		}
		/* Now cur_di points to 8.3 entry which corresponds to current
		 * file, so we need to get next entry for a next file */
	}

	return ret;
}

/* Fill given cluster number with zeroes */
static uint32_t fat_clear_clus(struct fat_fs_info *fsi,
		uint32_t clus, uint8_t *p_scratch) {
	uint32_t sec;

	assert(fsi);
	assert(p_scratch);

	sec = fat_sec_by_clus(fsi, clus);

	memset(p_scratch, 0, fsi->vi.bytepersec);
	for (int i = 0; i < fsi->vi.secperclus; i++) {
		if (fat_write_sector(fsi, p_scratch, sec + i)) {
			return DFS_ERRMISC;
		}
	}

	return 0;
}

static uint32_t fat_dir_extend(struct dirinfo *di) {
	struct fat_fs_info *fsi = di->fi.fsi;
	uint32_t clus;
	clus = fat_get_free_fat(fsi, di->p_scratch);
	if (clus == DFS_BAD_CLUS) {
		return DFS_ERRMISC;
	}

	if (0 != fat_clear_clus(fsi, clus, di->p_scratch)) {
		return DFS_ERRMISC;
	}

	fat_set_fat(fsi, di->p_scratch, di->currentcluster, clus);

	di->currentcluster = clus;
	di->currentsector = 0;
	di->currententry = 0; /* clus is not zero but contains fat entry,
				 so next loop will call */
	clus = fat_end_of_chain(fsi);

	fat_set_fat(fsi, di->p_scratch, di->currentcluster, clus);

	read_dir_buf(di);

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
static uint32_t fat_get_free_dir_ent(struct dirinfo *di, struct fat_dirent *de) {
	uint32_t ret;
	int offset = 0;

	do {
		ret = fat_get_current(di, de);

		if (ret == DFS_EOF) {
			return offset;
		}

		if (ret == DFS_OK) {
			if (de->name[0] == '\0') {
				return offset;
			}
		}

		offset++;
		di->currententry++;

		ret = fat_fetch_dir(di);
		if (ret == DFS_ALLOCNEW) {
			/* Need to allocate new cluster */
			if (DFS_OK != (ret = fat_dir_extend(di))) {
				return -1;
			}

			return offset;
		}
	} while (1);

	/* We shouldn't get here */
	return DFS_ERRMISC;
}

static void fat_set_direntry(uint32_t dir_cluster, uint32_t cluster) {
	struct fat_dirent *de = (struct fat_dirent *) fat_sector_buff;

	de[0] = (struct fat_dirent) {
		.name = MSDOS_DOT,
		.attr = ATTR_DIRECTORY,
	};
	fat_direntry_set_clus(&de[0], cluster);

	de[1] = (struct fat_dirent) {
		.name = MSDOS_DOTDOT,
		.attr = ATTR_DIRECTORY,
	};
	fat_direntry_set_clus(&de[1], dir_cluster);

	fat_set_filetime(&de[0]);
	fat_set_filetime(&de[1]);
}

int fat_root_dir_record(void *bdev) {
	uint32_t cluster;
	struct fat_fs_info fsi;
	uint32_t pstart, psize;
	uint8_t pactive, ptype;
	struct fat_dirent de;
	int dev_blk_size = block_dev(bdev)->block_size;
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
	};
	fat_direntry_set_clus(&de, cluster);

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

	cluster = fat_end_of_chain(&fsi);
	fat_set_fat(&fsi, fat_sector_buff, cluster, cluster);

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

	log_debug("len(%d) volinfo: secperclus(%d), bytepersec(%d)",
			len, fi->volinfo->secperclus, fi->volinfo->bytepersec );

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
			if (fat_is_end_of_chain(fsi, fi->cluster)) {
				result = DFS_EOF;
			} else {
				fi->cluster = fat_get_fat(fsi, p_scratch, fi->cluster);
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
	uint32_t new_clus = 0;
	struct fat_fs_info *fsi;
	fsi = fi->fsi;

	if (!(fi->mode & O_WRONLY) && !(fi->mode & O_APPEND) && !(fi->mode & O_RDWR)) {
		return DFS_ERRMISC;
	}

	log_debug("len(%d) volinfo: secperclus(%d), bytepersec(%d)",
			len, fi->volinfo->secperclus, fi->volinfo->bytepersec );

	if (fi->firstcluster == 0) {
		new_clus = fat_get_free_fat(fsi, fat_sector_buff);
		fat_set_fat(fsi, fat_sector_buff, new_clus, fat_end_of_chain(fsi));
		fi->firstcluster = fi->cluster = new_clus;
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
			fi->cluster = fat_get_fat(fsi, p_scratch, fi->cluster);

			/* Allocate a new cluster? */
			if (fat_is_end_of_chain(fsi, fi->cluster)) {
			  	uint32_t tempclus;
				tempclus = fat_get_free_fat(fsi, p_scratch);
				if (tempclus == DFS_BAD_CLUS)
					return DFS_ERRMISC;
				/* Link new cluster onto file */
				fat_set_fat(fsi, p_scratch, lastcluster, tempclus);
				fi->cluster = tempclus;
				tempclus = fat_end_of_chain(fsi);
				fat_set_fat(fsi, p_scratch, fi->cluster, tempclus);

				result = DFS_OK;
			}
		}
	}
	/* If cleared, then mark free clusters*/
	// TODO implement fat truncate
	if (0 && *size > fi->pointer) {
		if (div(*size, clastersize).quot !=
			div(fi->pointer, clastersize).quot) {

			nextcluster = fat_get_fat(fsi, p_scratch, fi->cluster);

			lastcluster = fat_end_of_chain(fsi);
			fat_set_fat(fsi, p_scratch, fi->cluster, lastcluster);

			/* Now follow the cluster chain to free the file space */
			while (!fat_is_end_of_chain(fsi, nextcluster)) {
				lastcluster = nextcluster;
				nextcluster = fat_get_fat(fsi, p_scratch, nextcluster);

				fat_set_fat(fsi, p_scratch, lastcluster, 0);
			}

		}
	}

	/* Update directory entry */
	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	if (new_clus != 0) {
		fat_direntry_set_clus(&((struct fat_dirent*) p_scratch)[fi->diroffset], new_clus);
	}

	fat_direntry_set_size(&((struct fat_dirent*) p_scratch)[fi->diroffset], *size);

	if (fat_write_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	return result;
}

static void fat_dir_clean_long(struct dirinfo *di, struct fat_file_info *fi) {
	struct fat_dirent de = { };
	struct dirinfo saved_di = { };
	struct dirinfo last_di = { };
	void *p_scratch = di->p_scratch;
	struct fat_fs_info *fsi = fi->fsi;

	fat_reset_dir(di);

	if (read_dir_buf(di)) {
		return;
	}

	while (true) {
		memcpy(&last_di, di, sizeof(last_di));

		fat_get_next(di, &de);

		if (fi->cluster == fat_direntry_get_clus(&de)) {
			if (saved_di.p_scratch == NULL) {
				/* Not a long entry */
				return;
			}

			read_dir_buf(&saved_di);

			while (saved_di.currententry != di->currententry ||
					saved_di.currentcluster != di->currentcluster ||
					saved_di.currentsector != di->currentsector) {
				((struct fat_dirent*) p_scratch)[saved_di.currententry].name[0] = 0xe5;

				if (fat_write_sector(fsi, p_scratch, fat_current_dirsector(&saved_di))) {
					return;
				}

				fat_get_next(&saved_di, &de);
			}

			return;
		}

		if (de.attr != ATTR_LONG_NAME) {
			memset(&saved_di, 0, sizeof(saved_di));
		} else if ((de.name[0] & FAT_LONG_ORDER_NUM_MASK) &&
				saved_di.p_scratch == NULL) {
			/* Save directory state only if it was not saved
			 * for previous entry */
			memcpy(&saved_di, &last_di, sizeof(saved_di));
		}
	}
}

static int fat_dir_empty(struct fat_file_info *fi) {
	struct dirinfo *di = (void *) fi;
	struct fat_dirent de = { };
	int res = 0;

	fat_reset_dir(di);

	if (read_dir_buf(di)) {
		return 0;
	}

	while (de.name[0] == '\0' && res != DFS_EOF) {
		res = fat_get_next(di, &de);
		if (!strncmp((void *) de.name, ".  ", 3) ||
			!strncmp((void *) de.name, ".. ", 3) ||
			de.name[0] == 0xe5 ||
			ATTR_LONG_NAME == (de.attr & ATTR_LONG_NAME)) {
			de.name[0] = '\0';
		}
	}

	return DFS_EOF == res;
}

/*
 * Delete a file
 * p_scratch must point to a sector-sized buffer
 */
int fat_unlike_file(struct fat_file_info *fi, uint8_t *p_scratch) {
	uint32_t tempclus;
	struct fat_fs_info *fsi;
	struct dirinfo *di = fi->fdi;

	fsi = fi->fsi;

	if (S_ISDIR(fi->mode) && !fat_dir_empty(fi)) {
		return -EPERM;
	}

	fat_dir_clean_long(di, fi);

	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	((struct fat_dirent*) p_scratch)[fi->diroffset].name[0] = 0xe5;
	if (fat_write_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	/* Now follow the cluster chain to free the file space */
	while (!fat_is_end_of_chain(fsi, fi->firstcluster)) {
		tempclus = fi->firstcluster;
		fi->firstcluster = fat_get_fat(fsi, p_scratch, fi->firstcluster);
		fat_set_fat(fsi, p_scratch, tempclus, 0);
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
			di->currentcluster = vi->rootdir / vi->secperclus;
		}

		di->currententry = 0;
	} else {
		fat_dirent_by_file(&di->fi, &de);

		di->currentcluster = fat_direntry_get_clus(&de);
		di->currentsector = 0;
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
	uint32_t cluster;
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
	log_debug("fsi: bytepersec(%d), bdev->block_size(%d)",
			fsi->vi.bytepersec, fsi->bdev->block_size);
	log_debug("name(%s), mode(%x)", name, mode);

	entries = fat_entries_per_name(name);

	di->fi.fsi = fsi;

	res = fat_get_free_entries(di, entries);
	if (res < 0) {
		return -1;
	}

	fat_dir_rewind(di, res);

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

			fat_write_de(di, &de);
			di->currententry++;
			if (DFS_EOF == fat_fetch_dir(di)) {
				fat_dir_extend(di);
			}
		}
	} else {
		memcpy(filename, name, sizeof(filename));
	}

	cluster = fat_get_free_fat(fsi, fat_sector_buff);
	de = (struct fat_dirent) {
		.attr = S_ISDIR(mode) ? ATTR_DIRECTORY : 0,
	};
	fat_direntry_set_clus(&de, cluster);
	memcpy(de.name, filename, MSDOS_NAME);
	fat_set_filetime(&de);

	fi->volinfo = volinfo;
	fi->pointer = 0;
	fi->dirsector = fat_current_dirsector(di);
	fi->diroffset = di->currententry;
	fi->cluster = cluster;
	fi->firstcluster = cluster;

	fat_write_de(di, &de);

	cluster = fat_end_of_chain(fsi);
	fat_set_fat(fsi, fat_sector_buff, fi->cluster, cluster);

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
	struct fat_dirent de;
	struct dirinfo *dir;
	int offt = 1;

	assert(name);
	assert(fi);

	log_debug("fi->dirsector (%d)", fi->dirsector);

	name[0] = '\0';

	dir = fi->fdi;
	fat_reset_dir(dir);
	read_dir_buf(dir);

	while (1) {
		int dirsect =fat_current_dirsector(dir);

		if (dirsect == fi->dirsector && dir->currententry == fi->diroffset + 1) {
			break;
		}

		fat_get_next_long(dir, &de, name);
	}

	if (name[0] == '\0') {
		/* Entry is the first in the directory */
		fat_get_next_long(dir, &de, name);
	}

	/* In FAT names by default are padded with zeroes,
	 * we don't want that in VFS tree */
	offt = strlen(name);
	while (name[offt - 1] == ' ' && offt > 0) {
		name[--offt] = '\0';
	}
	log_debug("name(%s)", name);

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

static uint32_t fat_dir_rewind(struct dirinfo *di, int n) {
	fat_reset_dir(di);
	read_dir_buf(di);

	for (int i = 0; i < n; i++) {
		di->currententry++;
		fat_fetch_dir(di);
	}

	return 0;
}

static uint32_t fat_get_free_entries(struct dirinfo *dir, int n) {
	struct fat_dirent de;
	uint32_t res;
	int offt = 0;
	bool failed;

	assert(dir);

	fat_reset_dir(dir);
	read_dir_buf(dir);

	while (true) {
		failed = false;

		/* Find some free entry */
		res = fat_get_free_dir_ent(dir, &de);
		if (res >= 0) {
			offt += res;
		} else {
			while (1);
		}

		if (res == DFS_EOF) {
			break;
		}

		/* Check if following N - 1 entries are free as well */
		for (int i = 0; i < n - 1; i++) {
			dir->currententry++;
			res = fat_fetch_dir(dir);
			if (res == DFS_ALLOCNEW) {
				/* Need to allocate new cluster */
				if (DFS_OK != (res = fat_dir_extend(dir))) {
					return -1;
				}
			}
			res = fat_get_current(dir, &de);
			if (res == DFS_OK && de.name[0] != '\0') {
				failed = true;
				offt += i + 1;
				break;
			}

			if (res == DFS_EOF) {
				break;
			}
		}

		if (failed) {
			continue;
		}

		break;
	}

	return offt;
}

static uint32_t fat_write_de(struct dirinfo *di, struct fat_dirent *de) {
	uint32_t sector;
	struct fat_fs_info *fsi = di->fi.fsi;

	assert(di);
	assert(de);

	sector = fat_current_dirsector(di);

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

#define DEFAULT_FAT_VERSION OPTION_GET(NUMBER, default_fat_version)
/**
 * @brief Format given block device
 * @param dev Pointer to device
 * @note Should be block device
 *
 * @return Negative error code or 0 if succeed
 */
int fat_format(struct block_dev *dev, void *priv) {
	int fat_n = priv ? atoi((char*) priv) : 0;
	struct block_dev *bdev = dev;

	assert(dev);

	if (!fat_n) {
		fat_n = DEFAULT_FAT_VERSION;
	}

	if (fat_n != 12 && fat_n != 16 && fat_n != 32) {
		log_error("Unsupported FAT version: FAT%d "
				"(FAT12/FAT16/FAT32 available)", fat_n);
		return -EINVAL;
	}

	fat_create_partition(bdev, fat_n);
	fat_root_dir_record(bdev);

	return 0;
}

extern struct inode_operations fat_iops;
extern struct super_block_operations fat_sbops;
extern struct file_operations fat_fops;
/* @brief Initializing fat super_block
 * @param sb  Structure to be initialized
 * @param dev Storage device
 *
 * @return Negative error code
 */
int fat_fill_sb(struct super_block *sb, const char *source) {
	struct fat_fs_info *fsi;
	struct block_dev *bdev;
	uint32_t pstart, psize;
	uint8_t pactive, ptype;
	struct dirinfo *di = NULL;
	int rc = 0;

	assert(sb);

	bdev = bdev_by_path(source);
	if (!bdev) {
		/* FAT always uses block device, so we can't fill superblock */
		return -ENOENT;
	}

	fsi = fat_fs_alloc();
	*fsi = (struct fat_fs_info) {
		.bdev = bdev,
	};
	sb->sb_data = fsi;
	sb->sb_iops = &fat_iops;
	sb->sb_fops = &fat_fops;
	sb->sb_ops  = &fat_sbops;
	sb->bdev    = bdev;

	/* Obtain pointer to first partition on first (only) unit */
	pstart = fat_get_ptn_start(bdev, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		rc = -EINVAL;
		goto error;
	}

	if (fat_get_volinfo(bdev, &fsi->vi, pstart)) {
		rc = -EBUSY;
		goto error;
	}

	if (NULL == (di = fat_dirinfo_alloc())) {
		rc = -ENOMEM;
		goto error;
	}
	memset(di, 0, sizeof(struct dirinfo));
	di->p_scratch = fat_sector_buff;

	if (fat_open_rootdir(fsi, di)) {
		rc = -EBUSY;
		goto error;
	}

	di->fi.fsi = fsi;
	di->fi.volinfo = &fsi->vi;

	inode_priv_set(sb->sb_root, di);
	sb->sb_root->i_ops = &fat_iops;

	return 0;

error:
	if (di) {
		fat_dirinfo_free(di);
	}

	fat_fs_free(fsi);
	return rc;
}

/**
 * @brief Set appropriate flags and i_data for given inode
 *
 * @param inode Inode to be filled
 * @param di FAT directory entry related to file
 *
 * @return Negative error code or zero if succeed
 */
int fat_fill_inode(struct inode *inode, struct fat_dirent *de, struct dirinfo *di) {
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct dirinfo *new_di;
	struct volinfo *vi;
	struct super_block *sb;
	int res, tmp_sector, tmp_entry, tmp_cluster;

	assert(de);
	assert(inode);
	assert(di);

	sb = inode->i_sb;
	assert(sb);

	fsi = sb->sb_data;
	assert(fsi);

	vi = &fsi->vi;
	assert(vi);

	/* Need to save some dirinfo data because this
	 * stuff may change while we traverse to the end
	 * of long name entry */
	tmp_sector = di->currentsector;
	tmp_entry = di->currententry;
	tmp_cluster = di->currentcluster;

	while (de->attr == ATTR_LONG_NAME) {
		res = fat_get_next(di, de);

		if (res != DFS_OK && res != DFS_ALLOCNEW) {
			return -EINVAL;
		}
	}

	if (de->attr & ATTR_DIRECTORY){
		if (NULL == (new_di = fat_dirinfo_alloc()))
			goto err_out;

		memset(new_di, 0, sizeof(struct dirinfo));
		new_di->p_scratch = fat_sector_buff;
		new_di->fi.mode = S_IFDIR;
		inode->i_mode |= S_IFDIR;

		new_di->currentcluster = fat_direntry_get_clus(de);

		fi = &new_di->fi;
	} else {
		if (NULL == (fi = fat_file_alloc())) {
			goto err_out;
		}

		inode->i_mode |= S_IFREG;
	}

	inode_priv_set(inode, fi);

	*fi = (struct fat_file_info) {
		.fsi = fsi,
		.volinfo = vi,
	};

	if (di->fi.dirsector == 0 && (vi->filesystem == FAT12 || vi->filesystem == FAT16)) {
		fi->dirsector = tmp_sector + tmp_cluster * vi->secperclus;
	} else {
		fi->dirsector = tmp_sector + fat_sec_by_clus(fsi, tmp_cluster);
	}

	fi->diroffset    = tmp_entry - 1;
	fi->cluster      = fat_direntry_get_clus(de);
	fi->firstcluster = fi->cluster;
	fi->filelen      = fat_direntry_get_size(de);
	fi->fdi          = di;

	inode_size_set(inode, fi->filelen);
	if (de->attr & ATTR_READ_ONLY) {
		inode->i_mode |= S_IRALL;
	} else {
		inode->i_mode |= S_IRWXA;
	}
	return 0;
err_out:
	return -1;
}

/* @brief Get next inode in directory
 * @param inode   Structure to be filled
 * @param parent  Inode of parent directory
 * @param dir_ctx Directory context
 *
 * @return Error code
 */
int fat_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx) {
	struct dirinfo *dirinfo;
	struct fat_dirent de;
	int res;

	assert(parent->i_sb);

	dirinfo = inode_priv(parent);
	dirinfo->currententry = (uintptr_t) ctx->fs_ctx;

	if (dirinfo->currententry == 0) {
		/* Need to get directory data from drive */
		fat_reset_dir(dirinfo);
	}

	read_dir_buf(dirinfo);

	while (((res = fat_get_next_long(dirinfo, &de, NULL)) ==  DFS_OK) || res == DFS_ALLOCNEW) {
		if (de.attr & ATTR_VOLUME_ID) {
			continue;
		}

		if (!memcmp(de.name, MSDOS_DOT, strlen(MSDOS_DOT)) ||
			!memcmp(de.name, MSDOS_DOTDOT, strlen(MSDOS_DOT))) {
			continue;
		}

		break;
	}

	switch (res) {
	case DFS_OK: {
		char tmp_name[128];

		if (0 > fat_fill_inode(next, &de, dirinfo)) {
			return -1;
		}
		if (DFS_OK != fat_read_filename(inode_priv(next), fat_sector_buff, tmp_name)) {
			return -1;
		}
		strncpy(name, tmp_name, NAME_MAX-1);
		name[NAME_MAX - 1] = '\0';

		ctx->fs_ctx = (void *) ((uintptr_t) dirinfo->currententry);
		return 0;
	}
	case DFS_EOF:
		/* Fall through */
	default:
		return -1;
	}
}

int fat_truncate(struct inode *node, off_t length) {
	assert(node);

	inode_size_set(node, length);

	/* TODO realloc blocks*/

	return 0;
}

/* @brief Create new file or directory
 * @param i_new Inode to be filled
 * @param i_dir Inode realted to the parent
 * @param mode  Used to figure out file type
 *
 * @return Negative error code
 */
int fat_create(struct inode *i_new, struct inode *i_dir, int mode) {
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct dirinfo *di;
	char *name;

	assert(i_dir && i_new);

	inode_size_set(i_new, 0);

	di = inode_priv(i_dir);

	name = inode_name(i_new);
	assert(name);

	/* TODO check file exists */
	assert(i_dir->i_sb);
	fsi = i_dir->i_sb->sb_data;

	fat_reset_dir(di);
	read_dir_buf(di);

	if (S_ISDIR(i_new->i_mode)) {
		struct dirinfo *new_di;
		new_di = fat_dirinfo_alloc();
		if (!new_di) {
			return -ENOMEM;
		}
		new_di->p_scratch = fat_sector_buff;
		fi = &new_di->fi;
	} else {
		fi = fat_file_alloc();
		if (!fi) {
			return -ENOMEM;
		}
	}

	inode_priv_set(i_new, fi);

	fi->volinfo = &fsi->vi;
	fi->fdi     = di;
	fi->fsi     = fsi;
	fi->mode   |= i_new->i_mode;

	if (0 != fat_create_file(fi, di, name, fi->mode)) {
		return -EIO;
	}

	return 0;
}

int fat_destroy_inode(struct inode *inode) {
	struct fat_file_info *fi;
	struct dirinfo *di;

	if (!inode_priv(inode)) {
		return 0;
	}

	if (S_ISDIR(inode->i_mode)) {
		di = inode_priv(inode);
		fat_dirinfo_free(di);
	} else {
		fi = inode_priv(inode);
		fat_file_free(fi);
	}

	return 0;
}
