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

#include <fs/file_desc.h>
#include <framework/mod/options.h>
#include <module/embox/fs/driver/ramfs.h>

#define MAX_FILE_SIZE     OPTION_MODULE_GET(embox__fs__driver__ramfs, NUMBER, ramfs_file_size)
#define RAMFS_FILES       OPTION_MODULE_GET(embox__fs__driver__ramfs, NUMBER, inode_quantity)
#define RAMFS_DESCRIPTORS OPTION_MODULE_GET(embox__fs__driver__ramfs, NUMBER, ramfs_quantity)
#define RAMFS_BUFFER_SIZE OPTION_MODULE_GET(embox__fs__driver__ramfs, NUMBER, ramfs_buffer_size)
#define FILESYSTEM_SIZE   (MAX_FILE_SIZE * RAMFS_FILES)

#define RAMFS_NAME_LEN    32

/* DOS attribute bits  */
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20
#define ATTR_LONG_NAME  \
	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

struct block_dev;

struct ramfs_fs_info {
	uint32_t numblocks;         /* number of block in volume */
	uint32_t block_size;        /* size of block */
	uint32_t block_per_file;    /* max number of blocks file size*/
	struct block_dev *bdev;
};

struct ramfs_file_info {
	int     length;
	int     index;                 /* number of file in FS*/
	int     mode;                  /* mode in which this file was opened */
	char    name[RAMFS_NAME_LEN];
	void   *inode;
	struct ramfs_fs_info *fsi;
};

extern struct file_operations ramfs_fops;
extern int ramfs_fill_sb(struct super_block *sb, const char *source);
extern int ramfs_format(struct block_dev *bdev, void *priv);
extern int ramfs_delete(struct inode *dir, struct inode *node);
extern int ramfs_truncate(struct inode *node, off_t length);
extern int ramfs_create(struct inode *i_new, struct inode *i_dir, int mode);

extern struct ramfs_file_info *ramfs_file_alloc(struct inode *node);

extern int ramfs_destroy_inode(struct inode *inode);

#endif /* RAMFS_H_ */
