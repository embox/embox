/**
 * @file
 * @brief
 *
 * @date 29.03.2012
 * @author Andrey Gazukin
 */

#ifndef FAT_H_
#define FAT_H_

#include <stdint.h>

#include <embox/block_dev.h>
#include <fs/mbr.h>

#define MSDOS_NAME      11
#define ROOT_DIR        "/"
#define DIR_SEPARATOR	'/'	/* character separating directory components*/

/* 32-bit error codes */
#define DFS_OK			0			/* no error */
#define DFS_EOF			1			/* end of file (not an error) */
#define DFS_WRITEPROT	2			/* volume is write protected */
#define DFS_NOTFOUND	3			/* path or file not found */
#define DFS_PATHLEN		4			/* path too long */
#define DFS_ALLOCNEW	5			/* must allocate new directory cluster */
#define DFS_ERRMISC		0xffffffff	/* generic error */

/* Internal subformat identifiers */
#define FAT12			0
#define FAT16			1
#define FAT32			2

/* DOS attribute bits  */
#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME \
	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

/*
 * 	Directory entry structure
 * 	note: if name[0] == 0xe5, this is a free dir entry
 * 	      if name[0] == 0x00, this is a free entry and all subsequent entries
 * 	      are free
 * 		  if name[0] == 0x05, the first character of the name is 0xe5
 *
 * 	Date format: bit 0-4  = day of month (1-31)
 * 	             bit 5-8  = month, 1=Jan..12=Dec
 * 				 bit 9-15 =	count of years since 1980 (0-127)
 * 	Time format: bit 0-4  = 2-second count, (0-29)
 * 	             bit 5-10 = minutes (0-59)
 * 				 bit 11-15= hours (0-23)
 */
struct dirent {
	uint8_t name[11];		/* filename */
	uint8_t attr;			/* attributes (see ATTR_* constant definitions) */
	uint8_t reserved;		/* reserved, must be 0 */
	uint8_t crttimetenth;	/* create time, 10ths of a second (0-199 are valid) */
	uint8_t crttime_l;		/* creation time low byte */
	uint8_t crttime_h;		/* creation time high byte */
	uint8_t crtdate_l;		/* creation date low byte */
	uint8_t crtdate_h;		/* creation date high byte */
	uint8_t lstaccdate_l;	/* last access date low byte */
	uint8_t lstaccdate_h;	/* last access date high byte */
	uint8_t startclus_h_l;	/* high word of first cluster, low byte (FAT32) */
	uint8_t startclus_h_h;	/* high word of first cluster, high byte (FAT32) */
	uint8_t wrttime_l;		/* last write time low byte */
	uint8_t wrttime_h;		/* last write time high byte */
	uint8_t wrtdate_l;		/* last write date low byte */
	uint8_t wrtdate_h;		/* last write date high byte */
	uint8_t startclus_l_l;	/* low word of first cluster, low byte */
	uint8_t startclus_l_h;	/* low word of first cluster, high byte */
	uint8_t filesize_0;		/* file size, low byte */
	uint8_t filesize_1;		/* */
	uint8_t filesize_2;		/* */
	uint8_t filesize_3;		/* file size, high byte */
};

/*
 *	BIOS Parameter Block structure (FAT12/16)
 */
struct bpb {
	uint8_t bytepersec_l;	/* bytes per sector low byte (0x00) */
	uint8_t bytepersec_h;	/* bytes per sector high byte (0x02) */
	uint8_t	secperclus;		/* sectors per cluster (1,2,4,8,16,32,64,128 are valid) */
	uint8_t reserved_l;		/* reserved sectors low byte */
	uint8_t reserved_h;		/* reserved sectors high byte */
	uint8_t numfats;		/* number of FAT copies (2) */
	uint8_t rootentries_l;	/* number of root dir entries low byte (0x00 normally) */
	uint8_t rootentries_h;	/* number of root dir entries high byte (0x02 normally) */
	uint8_t sectors_s_l;	/* small num sectors low byte */
	uint8_t sectors_s_h;	/* small num sectors high byte */
	uint8_t mediatype;		/* media descriptor byte */
	uint8_t secperfat_l;	/* sectors per FAT low byte */
	uint8_t secperfat_h;	/* sectors per FAT high byte */
	uint8_t secpertrk_l;	/* sectors per track low byte */
	uint8_t secpertrk_h;	/* sectors per track high byte */
	uint8_t heads_l;		/* heads low byte */
	uint8_t heads_h;		/* heads high byte */
	uint8_t hidden_0;		/* hidden sectors low byte */
	uint8_t hidden_1;		/* (note - this is the number of MEDIA sectors before */
	uint8_t hidden_2;		/* first sector of VOLUME - we rely on the MBR instead) */
	uint8_t hidden_3;		/* hidden sectors high byte */
	uint8_t sectors_l_0;	/* large num sectors low byte */
	uint8_t sectors_l_1;	/* */
	uint8_t sectors_l_2;	/* */
	uint8_t sectors_l_3;	/* large num sectors high byte */
};

/*
 *	Extended BIOS Parameter Block structure (FAT12/16)
 */
struct ebpb {
	uint8_t unit;			/* int 13h drive# */
	uint8_t head;			/* archaic, used by Windows NT-class OSes for flags */
	uint8_t signature;		/* 0x28 or 0x29 */
	uint8_t serial_0;		/* serial# */
	uint8_t serial_1;		/* serial# */
	uint8_t serial_2;		/* serial# */
	uint8_t serial_3;		/* serial# */
	uint8_t label[11];		/* volume label */
	uint8_t system[8];		/* filesystem ID */
};

/*
 *	Extended BIOS Parameter Block structure (FAT32)
 */
struct ebpb32 {
	uint8_t fatsize_0;		/* big FAT size in sectors low byte */
	uint8_t fatsize_1;		/* */
	uint8_t fatsize_2;		/* */
	uint8_t fatsize_3;		/* big FAT size in sectors high byte */
	uint8_t extflags_l;		/* extended flags low byte */
	uint8_t extflags_h;		/* extended flags high byte */
	uint8_t fsver_l;		/* filesystem version (0x00) low byte */
	uint8_t fsver_h;		/* filesystem version (0x00) high byte */
	uint8_t root_0;			/* cluster of root dir, low byte */
	uint8_t root_1;			/* */
	uint8_t root_2;			/* */
	uint8_t root_3;			/* cluster of root dir, high byte */
	uint8_t fsinfo_l;		/* sector pointer to FSINFO within reserved area, low byte (2) */
	uint8_t fsinfo_h;		/* sector pointer to FSINFO within reserved area, high byte (0) */
	uint8_t bkboot_l;		/* sector pointer to backup boot sector within reserved area, low byte (6) */
	uint8_t bkboot_h;		/* sector pointer to backup boot sector within reserved area, high byte (0) */
	uint8_t reserved[12];	/* reserved, should be 0 */
	uint8_t unit;			/* int 13h drive# */
	uint8_t head;			/* archaic, used by Windows NT-class OSes for flags */
	uint8_t signature;		/* 0x28 or 0x29 */
	uint8_t serial_0;		/* serial# */
	uint8_t serial_1;		/* serial# */
	uint8_t serial_2;		/* serial# */
	uint8_t serial_3;		/* serial# */
	uint8_t label[11];		/* volume label */
	uint8_t system[8];		/* filesystem ID */
};

/*
 *	Logical Boot Record structure (volume boot sector)
 */
struct lbr {
	uint8_t jump[3];		/* JMP instruction */
	uint8_t oemid[8];		/* OEM ID, space-padded */
	struct bpb bpb;				/* BIOS Parameter Block */
	union {
		struct ebpb ebpb;		/* FAT12/16 Extended BIOS Parameter Block */
		struct ebpb32 ebpb32;	/* FAT32 Extended BIOS Parameter Block */
	} ebpb;
	uint8_t code[420];		/* boot sector code */
	uint8_t sig_55;			/* 0x55 signature byte */
	uint8_t sig_aa;			/* 0xaa signature byte */
};

/*
 *	Volume information structure (Internal to DOSFS)
 */
struct volinfo {
	uint8_t unit;			/* unit on which this volume resides */
	uint8_t filesystem;		/* formatted filesystem */

/*
 * These two fields aren't very useful, so support for them has been commented
 * out to save memory. (Note that the "system" tag is not actually used by DOS
 * to determine filesystem type - that decision is made entirely on the basis
 * of how many clusters the drive contains. DOSFS works the same way).
 * See tag: OEMID in dosfs.c
 */

 /*	uint8_t oemid[9]; */		/* OEM ID ASCIIZ */
 /*	uint8_t system[9]; */		/* system ID ASCIIZ */
	uint8_t label[12];			/* volume label ASCIIZ */
	uint32_t startsector;		/* starting sector of filesystem */
	uint16_t bytepersec;		/* Bytes per sector */
	uint8_t secperclus;			/* sectors per cluster */
	uint16_t reservedsecs;		/* reserved sectors */
	uint32_t numsecs;			/* number of sectors in volume */
	uint32_t secperfat;			/* sectors per FAT */
	uint16_t rootentries;		/* number of root dir entries */

	uint32_t numclusters;		/* number of clusters on drive */

	/* The fields below are PHYSICAL SECTOR NUMBERS. */
	uint32_t fat1;				/* starting sector# of FAT copy 1 */
	uint32_t rootdir;			/* starting sector# of root directory (FAT12/FAT16) or cluster (FAT32) */
	uint32_t dataarea;			/* starting sector# of data area (cluster #2) */
};

/*
 *	Flags in DIRINFO.flags
 */
#define DFS_DI_BLANKENT		0x01	/* Searching for blank entry */

/*
 *	Directory search structure (Internal to DOSFS)
 */
struct dirinfo {
	uint32_t currentcluster;	/* current cluster in dir */
	uint8_t currentsector;		/* current sector in cluster */
	uint8_t currententry;		/* current dir entry in sector */
	uint8_t *p_scratch;			/* ptr to user-supplied scratch buffer (one sector) */
	uint8_t flags;				/* internal DOSFS flags */
};

struct fat_fs_info {
	struct volinfo vi;
	struct block_dev *bdev;
	struct node *root;
};

struct fat_file_info {
	struct volinfo *volinfo;		/* vol_info_t used to open this file */
	uint32_t dirsector;			/* physical sector containing dir entry of this file */
	uint8_t diroffset;			/* # of this entry within the dir sector */
	int mode;				    /* mode in which this file was opened */
	uint32_t firstcluster;		/* first cluster of file */
	//uint32_t filelen;			/* byte length of file */

	uint32_t cluster;			/* current cluster */
	uint32_t pointer;			/* current (BYTE) pointer */
};

void fat_set_filetime(struct dirent *de);
void fat_get_filename(char *tmppath, char *filename);
int fat_check_filename(char *filename);

extern char *path_canonical_to_dir(char *dest, char *src);
extern char *path_dir_to_canonical(char *dest, char *src, char dir);

#endif /* FAT_H_ */
