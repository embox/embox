/**
 * @file
 * @brief
 *
 * @date 15.03.2013
 * @author Andrey Gazukin
 */

#ifndef RAMFS_H_
#define RAMFS_H_

#include <stdint.h>

#include <drivers/block_dev.h>
#include <embox/unit.h>

#include <module/embox/fs/driver/ramfs.h>

#define MAX_FILE_SIZE     OPTION_MODULE_GET(embox__fs__driver__ramfs, NUMBER, ramfs_file_size)
#define RAMFS_FILES       OPTION_MODULE_GET(embox__fs__driver__ramfs, NUMBER, inode_quantity)
#define RAMFS_DESCRIPTORS OPTION_MODULE_GET(embox__fs__driver__ramfs, NUMBER, ramfs_descriptor_quantity)
#define FILESYSTEM_SIZE   (MAX_FILE_SIZE * RAMFS_FILES)

/* DOS attribute bits  */
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20
#define ATTR_LONG_NAME  \
	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

struct ramfs_fs_info {
	uint32_t numblocks;			/* number of block in volume */
	uint32_t block_size;		/* size of block */
	uint32_t block_per_file;	/* max number of blocks filesize*/
	struct block_dev *bdev;
};

#define RAMFS_NAME_LEN	32
struct ramfs_file_info {
	int     index;		        /* number of file in FS*/
	int     mode;			/* mode in which this file was opened */
	char    name[RAMFS_NAME_LEN];
	void *inode;
	struct ramfs_fs_info *fsi;
};

#endif /* RAMFS_H_ */
