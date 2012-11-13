/**
 * @file
 * @brief
 *
 * @date 12.11.2012
 * @author Andrey Gazukin
 */

#ifndef TMPFS_H_
#define TMPFS_H_

#define ROOT_DIR "/"

/* 32-bit error codes */
#define DFS_OK			0			/* no error */
#define DFS_EOF			1			/* end of file (not an error) */
#define DFS_WRITEPROT	2			/* volume is write protected */
#define DFS_NOTFOUND	3			/* path or file not found */
#define DFS_PATHLEN		4			/* path too long */
#define DFS_ALLOCNEW	5			/* must allocate new directory cluster */
#define DFS_ERRMISC		0xffffffff	/* generic error */


#define SECTOR_SIZE		512		/* sector size in bytes */


/* DOS attribute bits  */
#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | \
ATTR_VOLUME_ID)

/*
 *	File handle structure
 */
typedef struct vol_info {
	uint8_t label[12];			/* volume label ASCIIZ */
	uint8_t secperclus;			/* sectors per cluster */
	uint32_t numsecs;			/* number of sectors in volume */
	uint32_t secperfat;			/* sectors per FAT */
	uint32_t numclusters;		/* number of clusters on drive */
} vol_info_t;

typedef struct tmpfs_fileinfo {
	uint8_t mode;				/* mode in which this file was opened */
	uint32_t firstcluster;		/* first cluster of file */

	uint32_t filelen;			/* byte length of file */
	uint32_t cluster;			/* current cluster */
	uint32_t pointer;			/* current (BYTE) pointer */
} tmpfs_fileinfo_t;


typedef struct tmpfs_fs_description {
	dev_t devnum;
	uint8_t root_name[MAX_LENGTH_FILE_NAME];
	vol_info_t vi;
} tmpfs_fs_description_t;

typedef struct tmpfs_file_description {
	tmpfs_fileinfo_t fi;
	tmpfs_fs_description_t *fs;
} tmpfs_file_description_t;


#endif /* TMPFS_H_ */
