/**
 * @file
 * @brief
 *
 * @date 28.03.2012
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fs/fat.h>
#include <fs/ramdisk.h>
#include <fs/fs.h>
#include <fs/vfs.h>
#include <util/array.h>
#include <embox/device.h>
#include <mem/page.h>

static FILE *hostfile;
static char *start_addr;

static uint8_t sector[SECTOR_SIZE];
//static uint8_t sector2[SECTOR_SIZE];
static uint32_t bytecount;
//static uint32_t pstart, psize;
//static uint8_t pactive, ptype;
static vol_info_t vi;
//static dir_info_t di;
//static dir_ent_t de;
//static uint32_t cache;
static file_info_t fi;
//static uint8_t *p;
//static unsigned size_p;


/* File operations */

static void *fatfs_fopen(struct file_desc *desc);
static int fatfs_fclose(struct file_desc *desc);
static size_t fatfs_fread(void *buf, size_t size, size_t count, void *file);
static size_t fatfs_fwrite(const void *buf, size_t size, size_t count,
		void *file);
static int fatfs_fseek(void *file, long offset, int whence);
static int fatfs_ioctl(void *file, int request, va_list args);

static file_operations_t fatfs_fop = { fatfs_fopen, fatfs_fclose, fatfs_fread,
		fatfs_fwrite, fatfs_fseek, fatfs_ioctl };

/*
 * file_operation
 */
static void *fatfs_fopen(struct file_desc *desc) {
	node_t *nod;

	nod = desc->node;
	if(DFS_OK == fat_open_file(&vi, (uint8_t *)&nod->name, DFS_WRITE, sector, &fi)) {
		return desc;
	}
	return NULL;
}

static int fatfs_fseek(void *file, long offset, int whence) {
	return 0;
}

static int fatfs_fclose(struct file_desc *desc) {
	return 0;
}

static size_t fatfs_fread(void *buf, size_t size, size_t count, void *file) {
	size_t size_to_read;
	struct file_desc *desc;
	size_t rezult;

	size_to_read = size * count;
	desc = (struct file_desc *) file;

	rezult = fat_read_file(&fi, sector, buf, &bytecount, size_to_read);
	if (DFS_OK == rezult) {
		return bytecount;
	}
	return rezult;
}

static size_t fatfs_fwrite(const void *buf, size_t size,
		size_t count, void *file) {
	size_t size_to_write;
	struct file_desc *desc;
	size_t rezult;

	size_to_write = size * count;
	desc = (struct file_desc *) file;

	rezult = fat_write_file(&fi, sector, (uint8_t *)buf, &bytecount, size_to_write);
	if (DFS_OK == rezult) {
		return bytecount;
	}
	return rezult;
}

static int fatfs_ioctl(void *file, int request, va_list args) {
	return 0;
}

/* File system operations */

int fatfs_create(void *params);
static int fatfs_delete(const char *fname);
static int fatfs_init(void * par);
static int fatfs_mount(void * par);

static fsop_desc_t fatfs_fsop = { fatfs_init, fatfs_create, fatfs_delete,
		fatfs_mount };

static fs_drv_t fatfs_drv = { "vfat", &fatfs_fop, &fatfs_fsop };

const fs_drv_t *fatfs_get_fs(void) {
    return &fatfs_drv;
}

static int fatfs_init(void * par) {
	ramdisk_params_t *params;

	params = (ramdisk_params_t *) par;

	start_addr = params->start_addr;
	return 0;
}

static int fatfs_mount(void *par) {
	fs_drv_t *fsdrv;
	if (NULL != (fsdrv = filesystem_find_drv("ramfs"))) {
		fsdrv->fsop->mount(NULL);
	}

	return 0;
}

int fatfs_create(void *params) {
	ramdisk_params_t *par;
	node_t *nod;

	par = (ramdisk_params_t *) params;
	if (NULL == (nod = vfs_add_path(par->name, NULL))) {
		return -1;/*file already exist*/
	}

	nod->fs_type = &fatfs_drv;
	nod->file_info = (void *) &fatfs_fop;
	nod->attr = params;

	fatfs_init(params);
	fatfs_format (params);

	return 0;
}

#define LABEL "RAMDISK"
#define SYSTEM "FAT16"
int fatfs_format (void *params) {

	lbr_t lbr;
	size_t num_sect;
	ramdisk_params_t *par;
	uint16_t bytepersec, secperfat, rootentries;


	memset ((void *)&lbr.jump, 0x00, SECTOR_SIZE); /* set all by ZERO */

	memset ((void *)&lbr.oemid, 0x45, 0x08); /* EEEEEEEE */

	bytepersec = 0x200;
	lbr.bpb.bytepersec_l = (uint8_t)(bytepersec & 0xFF);
	lbr.bpb.bytepersec_h = (uint8_t)((bytepersec & 0xFF00) >> 8);
	lbr.bpb.secperclus = 0x01;
	lbr.bpb.reserved_l = 0x01; /* reserved sectors */
	lbr.bpb.numfats = 0x02;/* 2 FAT copy */
	lbr.bpb.mediatype = 0xF8;

	lbr.sig_55 = 0x55;
	lbr.sig_aa = 0xAA;


	par = (ramdisk_params_t *) params;


	num_sect = par->size / bytepersec;
	if (0xFFFF > num_sect)	{
		lbr.bpb.sectors_s_l = (uint8_t)(0x00000FF & num_sect);
		lbr.bpb.sectors_s_h = (uint8_t)(0x00000FF & (num_sect >> 8));
	}
	else {
		lbr.bpb.sectors_l_0 = (uint8_t)(0x00000FF & num_sect );
		lbr.bpb.sectors_l_1 = (uint8_t)(0x00000FF & (num_sect >> 8));
		lbr.bpb.sectors_l_2 = (uint8_t)(0x00000FF & (num_sect >> 16));
		lbr.bpb.sectors_l_3 = (uint8_t)(0x00000FF & (num_sect >> 24));
	}

	secperfat = (uint16_t)(0xFFFF & ((num_sect * 2 /
			bytepersec) + 1));
	lbr.bpb.secperfat_l = (uint8_t)(0x00FF & secperfat );
	lbr.bpb.secperfat_h = (uint8_t)(0x00FF & (secperfat >> 8));

	rootentries = (uint16_t)(0xFFFF & (num_sect / 2));
	lbr.bpb.rootentries_l = (uint8_t)(0x00FF & rootentries );
	lbr.bpb.rootentries_h = (uint8_t)(0x00FF & (rootentries >> 8));


	lbr.ebpb.ebpb.signature = 0x29;
	strcpy ((void *)&lbr.ebpb.ebpb.label, LABEL);
	strcpy ((void *)&lbr.ebpb.ebpb.system, SYSTEM);

	memcpy ((void *)&sector[0], (void *)&lbr, SECTOR_SIZE);

	return fat_write_sector(0, sector, 0, 1);
}

static int fatfs_delete(const char *fname) {
	return 0;
}


DECLARE_FILE_SYSTEM_DRIVER(fatfs_drv);

/**
 *	Get starting sector# of specified partition on drive #unit
 *	NOTE: This code ASSUMES an MBR on the disk.
 *	scratchsector should point to a SECTOR_SIZE scratch area
 *	Returns 0xffffffff for any error.
 *	If pactive is non-NULL, this function also returns partition active flag.
 *	If pptype is non-NULL, this function also returns the partition type.
 *	If psize is non-NULL, this function also returns the partition size.
 */
uint32_t fat_get_ptn_start(uint8_t unit, uint8_t *scratchsector, uint8_t pnum,
		uint8_t *pactive, uint8_t *pptype, uint32_t *psize)
{
	uint32_t result;
	p_mbr_t mbr = (p_mbr_t) scratchsector;

	/* DOS ptable supports maximum 4 partitions */
	if (pnum > 3) {
		return DFS_ERRMISC;
	}

	/* Read MBR from target media */
	if (fat_read_sector(unit,scratchsector,0,1)) {
		return DFS_ERRMISC;
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
 * Retrieve volume info from BPB and store it in a vol_info_t structure
 * You must provide the unit and starting sector of the filesystem, and
 * a pointer to a sector buffer for scratch
 * Attempts to read BPB and glean information about the FS from that.
 * Returns 0 OK, nonzero for any error.
 */
uint32_t fat_get_vol_info(uint8_t unit, uint8_t *scratchsector,
		uint32_t startsector, p_vol_info_t volinfo)
{
	p_lbr_t lbr = (p_lbr_t) scratchsector;
	volinfo->unit = unit;
	volinfo->startsector = startsector;

	if(fat_read_sector(unit,scratchsector,startsector,1)) {
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
	 * If secperfat is 0, we must be in a FAT32 volume; get secperfat
	 * from the FAT32 EBPB. The volume label and system ID string are also
	 * in different locations for FAT12/16 vs FAT32.
	 */
	volinfo->secperfat =  (uint16_t) lbr->bpb.secperfat_l |
		  (((uint16_t) lbr->bpb.secperfat_h) << 8);
	if (!volinfo->secperfat) {
		volinfo->secperfat = (uint32_t) lbr->ebpb.ebpb32.fatsize_0 |
		  (((uint32_t) lbr->ebpb.ebpb32.fatsize_1) << 8) |
		  (((uint32_t) lbr->ebpb.ebpb32.fatsize_2) << 16) |
		  (((uint32_t) lbr->ebpb.ebpb32.fatsize_3) << 24);

		memcpy(volinfo->label, lbr->ebpb.ebpb32.label, 11);
		volinfo->label[11] = 0;

	}
	else {
		memcpy(volinfo->label, lbr->ebpb.ebpb.label, 11);
		volinfo->label[11] = 0;

	}

	/* note: if rootentries is 0, we must be in a FAT32 volume. */
	volinfo->rootentries =  (uint16_t) lbr->bpb.rootentries_l |
		  (((uint16_t) lbr->bpb.rootentries_h) << 8);

	/* after extracting raw info we perform some useful precalculations */
	volinfo->fat1 = startsector + volinfo->reservedsecs;

	/**
	 * The calculation below is designed to round up the root directory size
	 * for FAT12/16 and to simply ignore the root directory for FAT32, since
	 * it's a normal, expandable file in that situation.
	 */

	if (volinfo->rootentries) {
		volinfo->rootdir = volinfo->fat1 + (volinfo->secperfat * 2);
		volinfo->dataarea = volinfo->rootdir + (((volinfo->rootentries * 32) +
				(SECTOR_SIZE - 1)) / SECTOR_SIZE);
	}
	else {
		volinfo->dataarea = volinfo->fat1 + (volinfo->secperfat * 2);
		volinfo->rootdir = (uint32_t) lbr->ebpb.ebpb32.root_0 |
		  (((uint32_t) lbr->ebpb.ebpb32.root_1) << 8) |
		  (((uint32_t) lbr->ebpb.ebpb32.root_2) << 16) |
		  (((uint32_t) lbr->ebpb.ebpb32.root_3) << 24);
	}

	/*
	 * Calculate number of clusters in data area and infer FAT type from
	 * this information.
	 */
	volinfo->numclusters =
			(volinfo->numsecs - volinfo->dataarea) / volinfo->secperclus;

	/* try to set FAT type by filesystem ID */
	if ((0 == strcmp ((const char *)lbr->ebpb.ebpb.system, "FAT12")) ||
			(0 == strcmp ((const char *)lbr->ebpb.ebpb32.system, "FAT12"))) {
		volinfo->filesystem = FAT12;
	}
	else if ((0 == strcmp ((const char *)lbr->ebpb.ebpb.system, "FAT16")) ||
				(0 == strcmp ((const char *)lbr->ebpb.ebpb32.system, "FAT16"))) {
			volinfo->filesystem = FAT16;
		}
	else if ((0 == strcmp ((const char *)lbr->ebpb.ebpb.system, "FAT32")) ||
				(0 == strcmp ((const char *)lbr->ebpb.ebpb32.system, "FAT32"))) {
			volinfo->filesystem = FAT32;
		}
	/* set FAT type by size */
	else if (volinfo->numclusters < 4085) {
		volinfo->filesystem = FAT12;
	}
	else if (volinfo->numclusters < 65525) {
		volinfo->filesystem = FAT16;
	}
	else {
		volinfo->filesystem = FAT32;
	}

	return DFS_OK;
}

/*
 *	Fetch FAT entry for specified cluster number You must provide a scratch
 *	buffer for one sector (SECTOR_SIZE) and a populated vol_info_t
 *	Returns a FAT32 BAD_CLUSTER value for any error, otherwise the contents
 *	of the desired FAT entry.
 *	scratchcache should point to a UINT32. This variable caches the physical
 *	sector number last read into the scratch buffer for performance
 *	enhancement reasons.
 */
uint32_t fat_get_fat_(p_vol_info_t volinfo, uint8_t *scratch,
		uint32_t *scratchcache, uint32_t cluster)
{
	uint32_t offset, sector, result;

	if (volinfo->filesystem == FAT12) {
		offset = cluster + (cluster / 2);
	}
	else if (volinfo->filesystem == FAT16) {
		offset = cluster * 2;
	}
	else if (volinfo->filesystem == FAT32) {
		offset = cluster * 4;
	}
	else
		return 0x0ffffff7;	/* FAT32 bad cluster */

	/*
	 * at this point, offset is the BYTE offset of the desired sector from
	 * the start of the FAT. Calculate the physical sector containing this
	 *  FAT entry.
	 */
	sector = ldiv(offset, SECTOR_SIZE).quot + volinfo->fat1;

	/* If this is not the same sector we last read, then read it into RAM */
	if (sector != *scratchcache) {
		if(fat_read_sector(volinfo->unit, scratch, sector, 1)) {
			/*
			 * avoid anyone assuming that this cache value is still valid,
			 * which might cause disk corruption
			 */
			*scratchcache = 0;
			return 0x0ffffff7;	/* FAT32 bad cluster */
		}
		*scratchcache = sector;
	}

	/*
	 * At this point, we "merely" need to extract the relevant entry.
	 * This is easy for FAT16 and FAT32, but a royal PITA for FAT12 as
	 * a single entry may span a sector boundary. The normal way around this is
	 * always to read two FAT sectors, but that luxury is (by design intent)
	 * unavailable to DOSFS.
	 */
	offset = ldiv(offset, SECTOR_SIZE).rem;

	if (volinfo->filesystem == FAT12) {
		/* Special case for sector boundary - Store last byte of current sector
		 * Then read in the next sector and put the first byte of that sector
		 * into the high byte of result.
		 */
		if (offset == SECTOR_SIZE - 1) {
			result = (uint32_t) scratch[offset];
			sector++;
			if(fat_read_sector(volinfo->unit, scratch, sector, 1)) {
				/*
				 * avoid anyone assuming that this cache value is still valid,
				 *  which might cause disk corruption
				 */
				*scratchcache = 0;
				return 0x0ffffff7;	/* FAT32 bad cluster */
			}
			*scratchcache = sector;
			/* Thanks to Claudio Leonel for pointing out this missing line. */
			result |= ((uint32_t) scratch[0]) << 8;
		}
		else {
			result = (uint32_t) scratch[offset] |
			  ((uint32_t) scratch[offset+1]) << 8;
		}
		if (cluster & 1) {
			result = result >> 4;
		}
		else {
			result = result & 0xfff;
		}
	}
	else if (volinfo->filesystem == FAT16) {
		result = (uint32_t) scratch[offset] |
		  ((uint32_t) scratch[offset+1]) << 8;
	}
	else if (volinfo->filesystem == FAT32) {
		result = ((uint32_t) scratch[offset] |
		  ((uint32_t) scratch[offset+1]) << 8 |
		  ((uint32_t) scratch[offset+2]) << 16 |
		  ((uint32_t) scratch[offset+3]) << 24) & 0x0fffffff;
	}
	else {
		result = 0x0ffffff7;	/* FAT32 bad cluster */
	}
	return result;
}


/*
 * Set FAT entry for specified cluster number
 * You must provide a scratch buffer for one sector (SECTOR_SIZE)
 * and a populated vol_info_t Returns DFS_ERRMISC for any error, otherwise
 * DFS_OK scratchcache should point to a UINT32. This variable caches the
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

uint32_t fat_set_fat_(p_vol_info_t volinfo, uint8_t *scratch,
		uint32_t *scratchcache, uint32_t cluster, uint32_t new_contents)
{
	uint32_t offset, sector, result;
	if (volinfo->filesystem == FAT12) {
		offset = cluster + (cluster / 2);
		new_contents &=0xfff;
	}
	else if (volinfo->filesystem == FAT16) {
		offset = cluster * 2;
		new_contents &=0xffff;
	}
	else if (volinfo->filesystem == FAT32) {
		offset = cluster * 4;
		new_contents &=0x0fffffff;	/* FAT32 is really "FAT28" */
	}
	else
		return DFS_ERRMISC;

	/*
	 * at this point, offset is the BYTE offset of the desired sector from
	 * the start of the FAT.
	 * Calculate the physical sector containing this FAT entry.
	 */
	sector = ldiv(offset, SECTOR_SIZE).quot + volinfo->fat1;

	/* If this is not the same sector we last read, then read it into RAM */
	if (sector != *scratchcache) {
		if(fat_read_sector(volinfo->unit, scratch, sector, 1)) {
			/*
			 * avoid anyone assuming that this cache value is still valid,
			 * which might cause disk corruption
			 */
			*scratchcache = 0;
			return DFS_ERRMISC;
		}
		*scratchcache = sector;
	}

	/*
	 * At this point, we "merely" need to extract the relevant entry.
	 * This is easy for FAT16 and FAT32, but a royal PITA for FAT12 as a
	 * single entry may span a sector boundary. The normal way around this
	 * is always to read two FAT sectors, but that luxury is (by design intent)
	 * unavailable to DOSFS.
	 */
	offset = ldiv(offset, SECTOR_SIZE).rem;

	if (volinfo->filesystem == FAT12) {

		/*
		 * If this is an odd cluster, pre-shift the desired new contents
		 * 4 bits to make the calculations below simpler
		 */
		if (cluster & 1)
			new_contents = new_contents << 4;

		/* Special case for sector boundary */
		if (offset == SECTOR_SIZE - 1) {

			/* Odd cluster: High 12 bits being set */
			if (cluster & 1) {
				scratch[offset] = (scratch[offset] & 0x0f) |
						(new_contents & 0xf0);
			}
			/* Even cluster: Low 12 bits being set */
			else {
				scratch[offset] = new_contents & 0xff;
			}
			result = fat_write_sector(volinfo->unit,
					scratch, *scratchcache, 1);
			/* mirror the FAT into copy 2 */
			if (DFS_OK == result) {
				result = fat_write_sector(volinfo->unit, scratch,
						(*scratchcache)+volinfo->secperfat, 1);
			}

			/*
			 * If we wrote that sector OK, then read in the subsequent sector
			 * and poke the first byte with the remainder of this FAT entry.
			 */
			if (DFS_OK == result) {
				(*scratchcache)++;
				result = fat_read_sector(volinfo->unit, scratch,
						*scratchcache, 1);
				if (DFS_OK == result) {
					/* Odd cluster: High 12 bits being set*/
					if (cluster & 1) {
						scratch[0] = new_contents & 0xff00;
					}
					/* Even cluster: Low 12 bits being set */
					else {
						scratch[0] = (scratch[0] & 0xf0) |
								(new_contents & 0x0f);
					}
					result = fat_write_sector(volinfo->unit,
							scratch, *scratchcache, 1);
					/* mirror the FAT into copy 2 */
					if (DFS_OK == result) {
						result = fat_write_sector(volinfo->unit, scratch,
								(*scratchcache)+volinfo->secperfat, 1);
					}
				}
				else {
					/*
					 * avoid anyone assuming that this cache value is still
					 * valid, which might cause disk corruption
					 */
					*scratchcache = 0;
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
				scratch[offset] = (scratch[offset] & 0x0f) |
						(new_contents & 0xf0);
				scratch[offset+1] = new_contents & 0xff00;
			}
			/* Even cluster: Low 12 bits being set */
			else {
				scratch[offset] = new_contents & 0xff;
				scratch[offset+1] = (scratch[offset+1] & 0xf0) |
						(new_contents & 0x0f);
			}
			result = fat_write_sector(volinfo->unit,
					scratch, *scratchcache, 1);
			/* mirror the FAT into copy 2 */
			if (DFS_OK == result) {
				result = fat_write_sector(volinfo->unit, scratch,
						(*scratchcache)+volinfo->secperfat, 1);
			}
		}
	}
	else if (volinfo->filesystem == FAT16) {
		scratch[offset] = (new_contents & 0xff);
		scratch[offset+1] = (new_contents & 0xff00) >> 8;
		result = fat_write_sector(volinfo->unit, scratch, *scratchcache, 1);
		/* mirror the FAT into copy 2 */
		if (DFS_OK == result) {
			result = fat_write_sector(volinfo->unit, scratch,
					(*scratchcache)+volinfo->secperfat, 1);
		}
	}
	else if (volinfo->filesystem == FAT32) {
		scratch[offset] = (new_contents & 0xff);
		scratch[offset+1] = (new_contents & 0xff00) >> 8;
		scratch[offset+2] = (new_contents & 0xff0000) >> 16;
		scratch[offset+3] = (scratch[offset+3] & 0xf0) |
				((new_contents & 0x0f000000) >> 24);
		/*
		 * Note well from the above: Per Microsoft's guidelines we preserve the
		 * upper 4 bits of the FAT32 cluster value. It's unclear what these
		 * bits will be used for;
		 * in every example I've encountered they are always zero.
		 */
		result = fat_write_sector(volinfo->unit, scratch, *scratchcache, 1);
		/* mirror the FAT into copy 2 */
		if (DFS_OK == result) {
			result = fat_write_sector(volinfo->unit, scratch,
					(*scratchcache)+volinfo->secperfat, 1);
		}
	}
	else {
		result = DFS_ERRMISC;
	}

	return result;
}

/*
 *	Convert a filename element from canonical (8.3) to directory entry (11)
 *	form src must point to the first non-separator character.
 *	dest must point to a 12-byte buffer.
 */
uint8_t *fat_canonical_to_dir(uint8_t *dest, uint8_t *src)
{
	uint8_t *destptr = dest;

	memset(dest, ' ', 11);
	dest[11] = 0;

	while (*src && (*src != DIR_SEPARATOR) && (destptr - dest < 11)) {
		if (*src >= 'a' && *src <='z') {
			*destptr++ = (*src - 'a') + 'A';
			src++;
		}
		else if (*src == '.') {
			src++;
			destptr = dest + 8;
		}
		else {
			*destptr++ = *src++;
		}
	}

	return dest;
}

/*
 * 	Find the first unused FAT entry
 * 	You must provide a scratch buffer for one sector (SECTOR_SIZE) and a
 * 	populated vol_info_t Returns a FAT32 BAD_CLUSTER value for any error,
 * 	otherwise the contents of the desired FAT entry.
 * 	Returns FAT32 bad_sector (0x0ffffff7) if there is no free cluster available
 */
uint32_t fat_get_free_fat_(p_vol_info_t volinfo, uint8_t *scratch)
{
	uint32_t i, result = 0xffffffff, scratchcache = 0;

	/*
	 * Search starts at cluster 2, which is the first usable cluster
	 * NOTE: This search can't terminate at a bad cluster, because there might
	 * legitimately be bad clusters on the disk.
	 */
	for (i=2; i < volinfo->numclusters; i++) {
		result = fat_get_fat_(volinfo, scratch, &scratchcache, i);
		if (!result) {
			return i;
		}
	}
	return 0x0ffffff7;		/* Can't find a free cluster */
}


/*
 * Open a directory for enumeration by fat_get_nextDirEnt
 * You must supply a populated vol_info_t (see fat_get_vol_info)
 * The empty string or a string containing only the directory separator are
 * considered to be the root directory.
 * Returns 0 OK, nonzero for any error.
 */
uint32_t fat_open_dir(p_vol_info_t volinfo,
		uint8_t *dirname, p_dir_info_t dirinfo)
{
	/* Default behavior is a regular search for existing entries */
	dirinfo->flags = 0;

	if ((!strlen((char *) dirname)) || (((strlen((char *) dirname) == 1) &&
			(dirname[0] == DIR_SEPARATOR)))) {
		if (volinfo->filesystem == FAT32) {
			dirinfo->currentcluster = volinfo->rootdir;
			dirinfo->currentsector = 0;
			dirinfo->currententry = 0;

			/* read first sector of directory */
			return fat_read_sector(volinfo->unit, dirinfo->scratch,
					volinfo->dataarea +
					((volinfo->rootdir - 2) * volinfo->secperclus), 1);
		}
		else {
			dirinfo->currentcluster = 0;
			dirinfo->currentsector = 0;
			dirinfo->currententry = 0;

			/* read first sector of directory */
			return fat_read_sector(volinfo->unit, dirinfo->scratch,
					volinfo->rootdir, 1);
		}
	}

	/*
	 * This is not the root directory.
	 * We need to find the start of this subdirectory. We do this by devious
	 * means, using our own companion function fat_get_next.
	 */
	else {
		uint8_t tmpfn[12];
		uint8_t *ptr = dirname;
		uint32_t result;
		dir_ent_t de;

		if (volinfo->filesystem == FAT32) {
			dirinfo->currentcluster = volinfo->rootdir;
			dirinfo->currentsector = 0;
			dirinfo->currententry = 0;

			/* read first sector of directory */
			if (fat_read_sector(volinfo->unit, dirinfo->scratch,
					volinfo->dataarea +
					((volinfo->rootdir - 2) * volinfo->secperclus), 1)) {
				return DFS_ERRMISC;
			}
		}
		else {
			dirinfo->currentcluster = 0;
			dirinfo->currentsector = 0;
			dirinfo->currententry = 0;

			/* read first sector of directory */
			if (fat_read_sector(volinfo->unit,
					dirinfo->scratch, volinfo->rootdir, 1)) {
				return DFS_ERRMISC;
			}
		}

		/* skip leading path separators */
		while (*ptr == DIR_SEPARATOR && *ptr) {
			ptr++;
		}

		/*
		 *  Scan the path from left to right, finding the start cluster
		 *  of each entry.  Observe that this code is inelegant, but obviates
		 *  the need for recursion.
		 */
		while (*ptr) {
			fat_canonical_to_dir(tmpfn, ptr);

			de.name[0] = 0;

			do {
				result = fat_get_next(volinfo, dirinfo, &de);
			} while (!result && memcmp(de.name, tmpfn, 11));

			if (!memcmp(de.name, tmpfn, 11) &&
					((de.attr & ATTR_DIRECTORY) == ATTR_DIRECTORY)) {
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

				if (fat_read_sector(volinfo->unit,
						dirinfo->scratch, volinfo->dataarea +
						((dirinfo->currentcluster - 2) *
								volinfo->secperclus), 1)) {
					return DFS_ERRMISC;
				}
			}
			else if (!memcmp(de.name, tmpfn, 11) &&
					!(de.attr & ATTR_DIRECTORY)) {
				return DFS_NOTFOUND;
			}

			/* seek to next item in list */
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
uint32_t fat_get_next(p_vol_info_t volinfo,
		p_dir_info_t dirinfo, p_dir_ent_t dirent)
{
	uint32_t tempint;	/* required by fat_get_fat_ */

	/* Do we need to read the next sector of the directory? */
	if (dirinfo->currententry >= SECTOR_SIZE / sizeof(dir_ent_t)) {
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
			if (dirinfo->currentsector * (SECTOR_SIZE / sizeof(dir_ent_t)) >=
					volinfo->rootentries) {
				return DFS_EOF;
			}

			/* Otherwise try to read the next sector */
			if (fat_read_sector(volinfo->unit, dirinfo->scratch,
					volinfo->rootdir + dirinfo->currentsector, 1)) {
				return DFS_ERRMISC;
			}
		}

		/* Normal handling */
		else {
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
				dirinfo->currentcluster = fat_get_fat_(volinfo,
						dirinfo->scratch, &tempint, dirinfo->currentcluster);
			}
			if (fat_read_sector(volinfo->unit,
					dirinfo->scratch, volinfo->dataarea +
					((dirinfo->currentcluster - 2) * volinfo->secperclus) +
					dirinfo->currentsector, 1)) {
				return DFS_ERRMISC;
			}
		}
	}

	memcpy(dirent, &(((p_dir_ent_t) dirinfo->scratch)[dirinfo->currententry]),
			sizeof(dir_ent_t));

	if (dirent->name[0] == 0) {		/* no more files in this directory */
		/* If this is a "find blank" then we can reuse this name. */
		if (dirinfo->flags & DFS_DI_BLANKENT) {
			return DFS_OK;
		}
		else {
			return DFS_EOF;
		}
	}

	if (dirent->name[0] == 0xe5) {	/* handle deleted file entries */
		dirent->name[0] = 0;
	}
	else if ((dirent->attr & ATTR_LONG_NAME) == ATTR_LONG_NAME) {
		dirent->name[0] = 0;
	}
	/* handle kanji filenames beginning with 0xE5*/
	else if (dirent->name[0] == 0x05) {
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
 * Note - di.scratch must be preinitialized to point to a sector scratch buffer
 * de is a scratch structure
 * Returns DFS_ERRMISC if a new entry could not be located or created
 * de is updated with the same return information you would expect
 * from fat_get_next
 */
uint32_t fat_get_free_dir_ent(p_vol_info_t volinfo, uint8_t *path,
		p_dir_info_t di, p_dir_ent_t de)
{
	uint32_t tempclus,i;

	if (fat_open_dir(volinfo, path, di)) {
		return DFS_NOTFOUND;
	}

	/* Set "search for empty" flag so fat_get_next knows what we're doing */
	di->flags |= DFS_DI_BLANKENT;

	/*
	 * We seek through the directory looking for an empty entry
	 * Note we are reusing tempclus as a temporary result holder.
	 */
	tempclus = 0;
	do {
		tempclus = fat_get_next(volinfo, di, de);

		/* Empty entry found */
		if (tempclus == DFS_OK && (!de->name[0])) {
			return DFS_OK;
		}
		/* End of root directory reached */
		else if (tempclus == DFS_EOF) {
			return DFS_ERRMISC;
		}

		else if (tempclus == DFS_ALLOCNEW) {
			tempclus = fat_get_free_fat_(volinfo, di->scratch);
			if (tempclus == 0x0ffffff7) {
				return DFS_ERRMISC;
			}

			/* write out zeroed sectors to the new cluster */
			memset(di->scratch, 0, SECTOR_SIZE);
			for (i=0;i<volinfo->secperclus;i++) {
				if (fat_write_sector(volinfo->unit, di->scratch,
						volinfo->dataarea + ((tempclus - 2) *
								volinfo->secperclus) + i, 1)) {
					return DFS_ERRMISC;
				}
			}
			/* Point old end cluster to newly allocated cluster */
			i = 0;
			fat_set_fat_(volinfo, di->scratch, &i,
					di->currentcluster, tempclus);

			/* Update dir_info_t so caller knows where to place the new file */
			di->currentcluster = tempclus;
			di->currentsector = 0;
			/* since the code coming after this expects to subtract 1 */
			di->currententry = 1;

			/* Mark newly allocated cluster as end of chain */
			switch(volinfo->filesystem) {
				case FAT12:		tempclus = 0xff8;	break;
				case FAT16:		tempclus = 0xfff8;	break;
				case FAT32:		tempclus = 0x0ffffff8;	break;
				default:		return DFS_ERRMISC;
			}
			fat_set_fat_(volinfo, di->scratch,
					&i, di->currentcluster, tempclus);
		}
	} while (!tempclus);

	/* We shouldn't get here */
	return DFS_ERRMISC;
}

/*
 * Open a file for reading or writing. You supply populated vol_info_t,
 * a path to the file, mode (DFS_READ or DFS_WRITE) and an empty fileinfo
 * structure. You also need to provide a pointer to a sector-sized scratch
 * buffer.
 * Returns various DFS_* error states. If the result is DFS_OK, fileinfo
 * can be used to access the file from this point on.
 */
uint32_t fat_open_file(p_vol_info_t volinfo, uint8_t *path, uint8_t mode,
		uint8_t *scratch, p_file_info_t fileinfo)
{
	uint8_t tmppath[MAX_PATH];
	uint8_t filename[12];
	uint8_t *p;
	dir_info_t di;
	dir_ent_t de;

	memset(fileinfo, 0, sizeof(file_info_t));

	/* save access mode */
	fileinfo->mode = mode;

	/* Get a local copy of the path. If it's longer than MAX_PATH, abort. */
	strncpy((char *) tmppath, (char *) path, MAX_PATH);
	tmppath[MAX_PATH - 1] = 0;
	if (strcmp((char *) path,(char *) tmppath)) {
		return DFS_PATHLEN;
	}

	/* strip leading path separators */
	while (tmppath[0] == DIR_SEPARATOR){
		strcpy((char *) tmppath, (char *) tmppath + 1);
	}

	/* Parse filename off the end of the supplied path */
	p = tmppath;
	while (*(p++));

	p--;
	while (p > tmppath && *p != DIR_SEPARATOR) {
		p--;
	}

	if (*p == DIR_SEPARATOR) {
		p++;
	}

	fat_canonical_to_dir(filename, p);

	if (p > tmppath) {
		p--;
	}

	if (*p == DIR_SEPARATOR || p == tmppath) {
		*p = 0;
	}

	/*
	 *  At this point, if our path was MYDIR/MYDIR2/FILE.EXT, filename = "FILE    EXT" and
	 *  tmppath = "MYDIR/MYDIR2".
	 */
	di.scratch = scratch;
	if (fat_open_dir(volinfo, tmppath, &di)) {
		return DFS_NOTFOUND;
	}

	while (!fat_get_next(volinfo, &di, &de)) {
		if (!memcmp(de.name, filename, 11)) {
			/* You can't use this function call to open a directory. */
			if (de.attr & ATTR_DIRECTORY){
				return DFS_NOTFOUND;
			}

			fileinfo->volinfo = volinfo;
			fileinfo->pointer = 0;
			/*
			 * The reason we store this extra info about the file is so that we
			 * can speedily update the file size, modification date, etc. on a
			 * file that is opened for writing.
			 */
			if (di.currentcluster == 0) {
				fileinfo->dirsector = volinfo->rootdir + di.currentsector;
			}
			else {
				fileinfo->dirsector = volinfo->dataarea +
						((di.currentcluster - 2) *
						volinfo->secperclus) + di.currentsector;
			}
			fileinfo->diroffset = di.currententry - 1;
			if (volinfo->filesystem == FAT32) {
				fileinfo->cluster = (uint32_t) de.startclus_l_l |
				  ((uint32_t) de.startclus_l_h) << 8 |
				  ((uint32_t) de.startclus_h_l) << 16 |
				  ((uint32_t) de.startclus_h_h) << 24;
			}
			else {
				fileinfo->cluster = (uint32_t) de.startclus_l_l |
				  ((uint32_t) de.startclus_l_h) << 8;
			}
			fileinfo->firstcluster = fileinfo->cluster;
			fileinfo->filelen = (uint32_t) de.filesize_0 |
			  ((uint32_t) de.filesize_1) << 8 |
			  ((uint32_t) de.filesize_2) << 16 |
			  ((uint32_t) de.filesize_3) << 24;

			return DFS_OK;
		}
	}

	/*
	 * At this point, we KNOW the file does not exist. If the file was opened
	 * with write access, we can create it.
	 */
	if (mode & DFS_WRITE) {
		uint32_t cluster, temp;

		/* Locate or create a directory entry for this file */
		if (DFS_OK != fat_get_free_dir_ent(volinfo, tmppath, &di, &de)) {
			return DFS_ERRMISC;
		}

		/* put sane values in the directory entry */
		memset(&de, 0, sizeof(de));
		memcpy(de.name, filename, 11);
		de.crttime_l = 0x20;	/* 01:01:00am, Jan 1, 2006. */
		de.crttime_h = 0x08;
		de.crtdate_l = 0x11;
		de.crtdate_h = 0x34;
		de.lstaccdate_l = 0x11;
		de.lstaccdate_h = 0x34;
		de.wrttime_l = 0x20;
		de.wrttime_h = 0x08;
		de.wrtdate_l = 0x11;
		de.wrtdate_h = 0x34;

		/* allocate a starting cluster for the directory entry */
		cluster = fat_get_free_fat_(volinfo, scratch);

		de.startclus_l_l = cluster & 0xff;
		de.startclus_l_h = (cluster & 0xff00) >> 8;
		de.startclus_h_l = (cluster & 0xff0000) >> 16;
		de.startclus_h_h = (cluster & 0xff000000) >> 24;

		/* update file_info_t for our caller's sake */
		fileinfo->volinfo = volinfo;
		fileinfo->pointer = 0;
		/*
		 * The reason we store this extra info about the file is so that we can
		 * speedily update the file size, modification date, etc. on a file
		 * that is opened for writing.
		 */
		if (di.currentcluster == 0) {
			fileinfo->dirsector = volinfo->rootdir + di.currentsector;
		}
		else {
			fileinfo->dirsector = volinfo->dataarea +
					((di.currentcluster - 2) * volinfo->secperclus) +
					di.currentsector;
		}
		fileinfo->diroffset = di.currententry - 1;
		fileinfo->cluster = cluster;
		fileinfo->firstcluster = cluster;
		fileinfo->filelen = 0;

		/*
		 * write the directory entry
		 * note that we no longer have the sector containing the directory
		 * entry, tragically, so we have to re-read it
		 */

		if (fat_read_sector(volinfo->unit, scratch, fileinfo->dirsector, 1)) {
			return DFS_ERRMISC;
		}
		memcpy(&(((p_dir_ent_t) scratch)[di.currententry-1]),
				&de, sizeof(dir_ent_t));
		if (fat_write_sector(volinfo->unit, scratch, fileinfo->dirsector, 1)) {
			return DFS_ERRMISC;
		}
		/* Mark newly allocated cluster as end of chain */
		switch(volinfo->filesystem) {
			case FAT12:		cluster = 0xff8;	break;
			case FAT16:		cluster = 0xfff8;	break;
			case FAT32:		cluster = 0x0ffffff8;	break;
			default:		return DFS_ERRMISC;
		}
		temp = 0;
		fat_set_fat_(volinfo, scratch, &temp, fileinfo->cluster, cluster);

		return DFS_OK;
	}

	return DFS_NOTFOUND;
}

/*
 * Read an open file
 * You must supply a prepopulated file_info_t as provided by fat_open_file,
 * and a pointer to a SECTOR_SIZE scratch buffer.
 * 	Note that returning DFS_EOF is not an error condition. This function
 * 	updates the	successcount field with the number of bytes actually read.
 */
uint32_t fat_read_file(p_file_info_t fileinfo, uint8_t *scratch,
		uint8_t *buffer, uint32_t *successcount, uint32_t len)
{
	uint32_t remain;
	uint32_t result;
	uint32_t sector;
	uint32_t bytesread;

	/* Don't try to read past EOF */
	if (len > fileinfo->filelen - fileinfo->pointer) {
		len = fileinfo->filelen - fileinfo->pointer;
	}

	result = DFS_OK;
	remain = len;
	*successcount = 0;

	while (remain && result == DFS_OK) {
		/* This is a bit complicated. The sector we want to read is addressed
		 * at a cluster granularity by the fileinfo->cluster member. The file
		 * pointer tells us how many extra sectors to add to that number.
		 */
		sector = fileinfo->volinfo->dataarea +
		  ((fileinfo->cluster - 2) * fileinfo->volinfo->secperclus) +
		  div(div(fileinfo->pointer,fileinfo->volinfo->secperclus *
				  SECTOR_SIZE).rem, SECTOR_SIZE).quot;

		/* Case 1 - File pointer is not on a sector boundary */
		if (div(fileinfo->pointer, SECTOR_SIZE).rem) {
			uint16_t tempreadsize;

			/* We always have to go through scratch in this case */
			result = fat_read_sector(fileinfo->volinfo->unit,
					scratch, sector, 1);

			/*
			 * This is the number of bytes that we actually care about in the
			 * sector just read.
			 */
			tempreadsize = SECTOR_SIZE -
					(div(fileinfo->pointer, SECTOR_SIZE).rem);

			/* Case 1A - We want the entire remainder of the sector. After this
			 * point, all passes through the read loop will be aligned on a
			 * sector boundary, which allows us to go through the optimal path
			 *  2A below.
			 */
		   	if (remain >= tempreadsize) {
				memcpy(buffer, scratch + (SECTOR_SIZE - tempreadsize),
						tempreadsize);
				bytesread = tempreadsize;
				buffer += tempreadsize;
				fileinfo->pointer += tempreadsize;
				remain -= tempreadsize;
			}
			/* Case 1B - This read concludes the file read operation */
			else {
				memcpy(buffer, scratch + (SECTOR_SIZE - tempreadsize), remain);

				buffer += remain;
				fileinfo->pointer += remain;
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
			 if (remain >= SECTOR_SIZE) {
				result = fat_read_sector(fileinfo->volinfo->unit,
						buffer, sector, 1);
				remain -= SECTOR_SIZE;
				buffer += SECTOR_SIZE;
				fileinfo->pointer += SECTOR_SIZE;
				bytesread = SECTOR_SIZE;
			}
			/* Case 2B - We are only reading a partial sector */
			else {
				result = fat_read_sector(fileinfo->volinfo->unit,
						scratch, sector, 1);
				memcpy(buffer, scratch, remain);
				buffer += remain;
				fileinfo->pointer += remain;
				bytesread = remain;
				remain = 0;
			}
		}

		*successcount += bytesread;

		/* check to see if we stepped over a cluster boundary */
		if (div(fileinfo->pointer - bytesread,
				fileinfo->volinfo->secperclus * SECTOR_SIZE).quot !=
				div(fileinfo->pointer,
						fileinfo->volinfo->secperclus * SECTOR_SIZE).quot) {
			/*
			 * An act of minor evil - we use bytesread as a scratch integer,
			 * knowing that its value is not used after updating *successcount
			 * above
			 */
			bytesread = 0;
			if (((fileinfo->volinfo->filesystem == FAT12) &&
					(fileinfo->cluster >= 0xff8)) ||
					((fileinfo->volinfo->filesystem == FAT16) &&
							(fileinfo->cluster >= 0xfff8)) ||
							((fileinfo->volinfo->filesystem == FAT32) &&
									(fileinfo->cluster >= 0x0ffffff8))) {
				result = DFS_EOF;
			}
			else {
				fileinfo->cluster = fat_get_fat_(fileinfo->volinfo,
						scratch, &bytesread, fileinfo->cluster);
			}
		}
	}

	return result;
}

/*
 * Seek file pointer to a given position
 * This function does not return status - refer to the fileinfo->pointer value
 * to see where the pointer wound up.
 * Requires a SECTOR_SIZE scratch buffer
 */
void fat_seek(p_file_info_t fileinfo, uint32_t offset, uint8_t *scratch)
{
	uint32_t tempint;

	/* Case 0a - Return immediately for degenerate case */
	if (offset == fileinfo->pointer) {
		return;
	}

	/* Case 0b - Don't allow the user to seek past the end of the file */
	if (offset > fileinfo->filelen) {
		offset = fileinfo->filelen;
		/* NOTE NO RETURN HERE! */
	}

	/*
	 * Case 1 - Simple rewind to start
	 * Note _intentional_ fallthrough from Case 0b above
	 */
	if (offset == 0) {
		fileinfo->cluster = fileinfo->firstcluster;
		fileinfo->pointer = 0;
		return;
	}
	/* Case 2 - Seeking backwards. Need to reset and seek forwards */
	else if (offset < fileinfo->pointer) {
		fileinfo->cluster = fileinfo->firstcluster;
		fileinfo->pointer = 0;
		/* NOTE NO RETURN HERE! */
	}

	/*
	 * Case 3 - Seeking forwards
	 * Note _intentional_ fallthrough from Case 2 above
	 * Case 3a - Seek size does not cross cluster boundary -
	 * very simple case
	 */
	if (div(fileinfo->pointer,
			fileinfo->volinfo->secperclus * SECTOR_SIZE).quot ==
			div(fileinfo->pointer + offset, fileinfo->volinfo->secperclus *
					SECTOR_SIZE).quot) {
		fileinfo->pointer = offset;
	}
	/*Case 3b - Seeking across cluster boundary(ies) */
	else {
		/* round file pointer down to cluster boundary */
		fileinfo->pointer = div(fileinfo->pointer,
				fileinfo->volinfo->secperclus * SECTOR_SIZE).quot *
		  fileinfo->volinfo->secperclus * SECTOR_SIZE;

		/* seek by clusters */
		while (div(fileinfo->pointer,
				fileinfo->volinfo->secperclus * SECTOR_SIZE).quot !=
				div(fileinfo->pointer + offset,
						fileinfo->volinfo->secperclus * SECTOR_SIZE).quot) {

			fileinfo->cluster = fat_get_fat_(fileinfo->volinfo, scratch,
					&tempint, fileinfo->cluster);
			/* Abort if there was an error */
			if (fileinfo->cluster == 0x0ffffff7) {
				fileinfo->pointer = 0;
				fileinfo->cluster = fileinfo->firstcluster;
				return;
			}
			fileinfo->pointer += SECTOR_SIZE * fileinfo->volinfo->secperclus;
		}

		/* since we know the cluster is right, we have no more work to do */
		fileinfo->pointer = offset;
	}
}

/*
 * Delete a file
 * scratch must point to a sector-sized buffer
 */
uint32_t fat_unlike_file(p_vol_info_t volinfo, uint8_t *path, uint8_t *scratch)
{
	file_info_t fi;
	uint32_t cache;
	uint32_t tempclus;

	cache = 0;
	/* fat_open_file gives us all the information we need to delete it */
	if (DFS_OK != fat_open_file(volinfo, path, DFS_READ, scratch, &fi)) {
		return DFS_NOTFOUND;
	}

	/* First, read the directory sector and delete that entry */
	if (fat_read_sector(volinfo->unit, scratch, fi.dirsector, 1)) {
		return DFS_ERRMISC;
	}
	((p_dir_ent_t) scratch)[fi.diroffset].name[0] = 0xe5;
	if (fat_write_sector(volinfo->unit, scratch, fi.dirsector, 1)) {
		return DFS_ERRMISC;
	}

	/* Now follow the cluster chain to free the file space */
	while (!((volinfo->filesystem == FAT12 && fi.firstcluster >= 0x0ff7) ||
			(volinfo->filesystem == FAT16 && fi.firstcluster >= 0xfff7) ||
			(volinfo->filesystem == FAT32 && fi.firstcluster >= 0x0ffffff7))) {
		tempclus = fi.firstcluster;

		fi.firstcluster = fat_get_fat_(volinfo, scratch,
				&cache, fi.firstcluster);

		fat_set_fat_(volinfo, scratch, &cache, tempclus, 0);

	}
	return DFS_OK;
}

/*
 * Write an open file
 * You must supply a prepopulated file_info_t as provided by
 * fat_open_file, and a pointer to a SECTOR_SIZE scratch buffer.
 * This function updates the successcount field with the number
 * of bytes actually written.
 */
uint32_t fat_write_file(p_file_info_t fileinfo, uint8_t *scratch,
		uint8_t *buffer, uint32_t *successcount, uint32_t len)
{
	uint32_t remain;
	uint32_t result = DFS_OK;
	uint32_t sector;
	uint32_t byteswritten;

	/* Don't allow writes to a file that's open as readonly */
	if (!(fileinfo->mode & DFS_WRITE)) {
		return DFS_ERRMISC;
	}

	remain = len;
	*successcount = 0;

	while (remain && result == DFS_OK) {
		/*
		 * This is a bit complicated. The sector we want to read is addressed
		 * at a cluster granularity by  the fileinfo->cluster member.
		 * The file pointer tells us how many extra sectors to add to that
		 * number.
		 */
		sector = fileinfo->volinfo->dataarea +
		  ((fileinfo->cluster - 2) * fileinfo->volinfo->secperclus) +
		  div(div(fileinfo->pointer,fileinfo->volinfo->secperclus *
				  SECTOR_SIZE).rem, SECTOR_SIZE).quot;

		/* Case 1 - File pointer is not on a sector boundary */
		if (div(fileinfo->pointer, SECTOR_SIZE).rem) {
			uint16_t tempsize;

			/* We always have to go through scratch in this case */
			result = fat_read_sector(fileinfo->volinfo->unit,
					scratch, sector, 1);

			/*
			 * This is the number of bytes that we don't want to molest in the
			 * scratch sector just read.
			 */
			tempsize = div(fileinfo->pointer, SECTOR_SIZE).rem;

			/*
			 * Case 1A - We are writing the entire remainder of the sector.
			 * After this point, all passes through the read loop will be
			 * aligned on a sector boundary, which allows us to go through the
			 * optimal path
			 * 2A below.
			 */
		   	if (remain >= SECTOR_SIZE - tempsize) {
				memcpy(scratch + tempsize, buffer, SECTOR_SIZE - tempsize);
				if (!result) {
					result = fat_write_sector(fileinfo->volinfo->unit,
							scratch, sector, 1);
				}

				byteswritten = SECTOR_SIZE - tempsize;
				buffer += SECTOR_SIZE - tempsize;
				fileinfo->pointer += SECTOR_SIZE - tempsize;
				if (fileinfo->filelen < fileinfo->pointer) {
					fileinfo->filelen = fileinfo->pointer;
				}
				remain -= SECTOR_SIZE - tempsize;
			}
			/* Case 1B - This concludes the file write operation */
			else {
				memcpy(scratch + tempsize, buffer, remain);
				if (!result) {
					result = fat_write_sector(fileinfo->volinfo->unit,
							scratch, sector, 1);
				}

				buffer += remain;
				fileinfo->pointer += remain;
				if (fileinfo->filelen < fileinfo->pointer) {
					fileinfo->filelen = fileinfo->pointer;
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
			if (remain >= SECTOR_SIZE) {
				result = fat_write_sector(fileinfo->volinfo->unit,
						buffer, sector, 1);
				remain -= SECTOR_SIZE;
				buffer += SECTOR_SIZE;
				fileinfo->pointer += SECTOR_SIZE;
				if (fileinfo->filelen < fileinfo->pointer) {
					fileinfo->filelen = fileinfo->pointer;
				}
				byteswritten = SECTOR_SIZE;
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
				if (fileinfo->pointer < fileinfo->filelen) {
					result = fat_read_sector(fileinfo->volinfo->unit,
							scratch, sector, 1);
					if (!result) {
						memcpy(scratch, buffer, remain);
						result = fat_write_sector(fileinfo->volinfo->unit,
								scratch, sector, 1);
					}
				}
				else {
					result = fat_write_sector(fileinfo->volinfo->unit,
							buffer, sector, 1);
				}

				buffer += remain;
				fileinfo->pointer += remain;
				if (fileinfo->filelen < fileinfo->pointer) {
					fileinfo->filelen = fileinfo->pointer;
				}
				byteswritten = remain;
				remain = 0;
			}
		}

		*successcount += byteswritten;

		/* check to see if we stepped over a cluster boundary */
		if (div(fileinfo->pointer - byteswritten,
				fileinfo->volinfo->secperclus * SECTOR_SIZE).quot !=
				div(fileinfo->pointer, fileinfo->volinfo->secperclus *
						SECTOR_SIZE).quot) {
		  	uint32_t lastcluster;

		  	/* We've transgressed into another cluster. If we were already
		  	 * at EOF, we need to allocate a new cluster.
		  	 * An act of minor evil - we use byteswritten as a scratch integer,
		  	 * knowing that its value is not used after updating *successcount
		  	 * above
		  	 */
		  	byteswritten = 0;

			lastcluster = fileinfo->cluster;
			fileinfo->cluster = fat_get_fat_(fileinfo->volinfo, scratch,
					&byteswritten, fileinfo->cluster);

			/* Allocate a new cluster? */
			if (((fileinfo->volinfo->filesystem == FAT12) &&
					(fileinfo->cluster >= 0xff8)) ||
					((fileinfo->volinfo->filesystem == FAT16) &&
					(fileinfo->cluster >= 0xfff8)) ||
					((fileinfo->volinfo->filesystem == FAT32) &&
					(fileinfo->cluster >= 0x0ffffff8))) {
			  	uint32_t tempclus;

				tempclus = fat_get_free_fat_(fileinfo->volinfo, scratch);
				byteswritten = 0; /* invalidate cache */
				if (tempclus == 0x0ffffff7) {
					return DFS_ERRMISC;
				}
				/* Link new cluster onto file */
				fat_set_fat_(fileinfo->volinfo, scratch, &byteswritten,
						lastcluster, tempclus);
				fileinfo->cluster = tempclus;

				/* Mark newly allocated cluster as end of chain */
				switch(fileinfo->volinfo->filesystem) {
					case FAT12:		tempclus = 0xff8;	break;
					case FAT16:		tempclus = 0xfff8;	break;
					case FAT32:		tempclus = 0x0ffffff8;	break;
					default:		return DFS_ERRMISC;
				}
				fat_set_fat_(fileinfo->volinfo, scratch, &byteswritten,
						fileinfo->cluster, tempclus);

				result = DFS_OK;
			}
			/* No else clause is required. */
		}
	}

	/* Update directory entry */
		if (fat_read_sector(fileinfo->volinfo->unit, scratch,
				fileinfo->dirsector, 1)) {
			return DFS_ERRMISC;
		}

		((p_dir_ent_t) scratch)[fileinfo->diroffset].filesize_0 =
				fileinfo->filelen & 0xff;

		((p_dir_ent_t) scratch)[fileinfo->diroffset].filesize_1 =
				(fileinfo->filelen & 0xff00) >> 8;

		((p_dir_ent_t) scratch)[fileinfo->diroffset].filesize_2 =
				(fileinfo->filelen & 0xff0000) >> 16;

		((p_dir_ent_t) scratch)[fileinfo->diroffset].filesize_3 =
				(fileinfo->filelen & 0xff000000) >> 24;

		if (fat_write_sector(fileinfo->volinfo->unit, scratch,
				fileinfo->dirsector, 1)) {
			return DFS_ERRMISC;
		}

	return result;
}

int fat_attach_file(char *imagefile)
{
	hostfile = fopen(imagefile, "r+b");
	if (hostfile == NULL)
		return -1;

	return 0;	/* OK */
}

int fat_read_sector(uint8_t unit, uint8_t *buffer,
		uint32_t sector, uint32_t count)
{
	char *read_addr;
	uint32_t size;

	size = SECTOR_SIZE * count;
	read_addr = start_addr + (sector * SECTOR_SIZE);

	memcpy(buffer, read_addr, size);
	return DFS_OK;
}


int fat_write_sector(uint8_t unit, uint8_t *buffer,
		uint32_t sector, uint32_t count)
{
	char *write_addr;
	uint32_t size;

	size = SECTOR_SIZE * count;
	write_addr = start_addr + (sector * SECTOR_SIZE);

	memcpy(write_addr, buffer, size);
	return DFS_OK;
}

#ifdef _GAZ_DEBUG_

int fat_main(const void *name)
{
	uint8_t sector[SECTOR_SIZE], sector2[SECTOR_SIZE];
	uint32_t pstart, psize, i;
	uint8_t pactive, ptype;
	vol_info_t vi;
	//dir_info_t di;
	//dir_ent_t de;
	uint32_t cache;
	file_info_t fi;
	uint8_t *p;
	unsigned size_p;

	/* Attach user-specified image file
	if (fat_attach_file((char *) name)) {
		printf("Cannot attach image file '%s'\n", (char *)name);
		return -1;
	}*/

	/* Obtain pointer to first partition on first (only) unit */
	pstart = fat_get_ptn_start(0, sector, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		printf("Cannot find first partition\n");
		return -1;
	}

	if (fat_get_vol_info(0, sector, pstart, &vi)) {
		printf("Error getting volume information\n");
		return -1;
	}
	printf("Volume label '%s'\n", vi.label);
	printf("%d sector/s per cluster, %d reserved sector/s, volume total %d sectors.\n", vi.secperclus, vi.reservedsecs, vi.numsecs);
	printf("%d sectors per FAT, first FAT at sector #%d, root dir at #%d.\n",vi.secperfat,vi.fat1,vi.rootdir);
	printf("(For FAT32, the root dir is a CLUSTER number, FAT12/16 it is a SECTOR number)\n");
	printf("%d root dir entries, data area commences at sector #%d.\n",vi.rootentries,vi.dataarea);
	printf("%d clusters (%d bytes) in data area, filesystem IDd as ", vi.numclusters, vi.numclusters * vi.secperclus * SECTOR_SIZE);
	if (vi.filesystem == FAT12)
		printf("FAT12.\n");
	else if (vi.filesystem == FAT16)
		printf("FAT16.\n");
	else if (vi.filesystem == FAT32)
		printf("FAT32.\n");
	else
		printf("[unknown]\n");

/*------------------------------------------------------------*/
/* Directory enumeration test
	di.scratch = sector;

	if (fat_open_dir(&vi, (uint8_t *)"MYDIR1", &di)) {
		printf("error opening subdirectory\n");
		return -1;
	}
	while (!fat_get_next(&vi, &di, &de)) {
		if (de.name[0])
			printf("file: '%-11.11s'\n", de.name);
	}

*/
/*------------------------------------------------------------*/
/* File write test */
	/*if (fat_open_file(&vi, (uint8_t *)"MYDIR1/WRTEST.TXT", DFS_WRITE, sector, &fi)) {
		printf("error opening file\n");
		return -1;
	}*/
	/*if (fat_open_file(&vi, (uint8_t *)name, DFS_WRITE, sector, &fi)) {
			printf("error opening file\n");
			return -1;
		}*/
	for (i=0;i<18;i++) {
		memset(sector2, 128+i, SECTOR_SIZE);
		fat_write_file(&fi, sector, sector2, &cache, SECTOR_SIZE/2);
		memset(sector2+256, 255-i, SECTOR_SIZE/2);
		fat_write_file(&fi, sector, sector2+256, &cache, SECTOR_SIZE/2);
	}
	sprintf((char *)sector2, (const char *)"test string at the end...");
	fat_write_file(&fi, sector, sector2, &cache, strlen((const char *)sector2));

/*------------------------------------------------------------*/
/* File read test */
	printf("Readback test\n");
	if (fat_open_file(&vi, (uint8_t *)"MYDIR1/WRTEST.TXT", DFS_READ, sector, &fi)) {
		printf("error opening file\n");
		return -1;
	}

	size_p = (fi.filelen + 512) / CONFIG_PAGE_SIZE;
	size_p++; /*fractional part of pagesize*/

	p = (void *)page_alloc(size_p);
	memset(p, 0xaa, fi.filelen + 512);

	fat_read_file(&fi, sector, p, &i, fi.filelen);
    printf("read complete %d bytes (expected %d) pointer %d\n", i, fi.filelen, fi.pointer);

	{
		FILE *fp;
		fp = fopen("test.txt","wb");
		fwrite(p, fi.filelen + 512, 1, fp);
		fclose(fp);
	}

	return 0;
}
#endif /*def _GAZ_DEBUG_ */
