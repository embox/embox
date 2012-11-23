/**
 * @file
 * @brief
 *
 * @date 12.11.2012
 * @author Andrey Gazukin
 */

#ifndef TMPFS_H_
#define TMPFS_H_

/* DOS attribute bits  */
#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | \
ATTR_VOLUME_ID)

typedef struct tmpfs_filesystem {
	void *dev_id;
	uint8_t root_name[MAX_LENGTH_PATH_NAME];
	uint32_t numblocks;			/* number of block in volume */
	uint32_t block_size;		/* size of block */
	uint32_t block_per_file;	/* max number of blocks filesize*/
} tmpfs_filesystem_t;

typedef struct tmpfs_file_info {
	int     index;		        /* number of file in FS*/
	uint8_t mode;				/* mode in which this file was opened */
	uint32_t filelen;			/* byte length of file */
	uint32_t pointer;			/* current (BYTE) pointer */
	tmpfs_filesystem_t *fs;
} tmpfs_file_info_t;


#endif /* TMPFS_H_ */
