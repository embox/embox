/**
 * @file
 *
 * @date Dec 23, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_FS_DRIVER_INITFS_INITFS_H_
#define SRC_FS_DRIVER_INITFS_INITFS_H_

#include <stddef.h>

struct dir_ctx;
struct inode;
struct cpio_entry;

struct initfs_file_info {
	uintptr_t start_pos;
	struct cpio_entry *entry;

	/* Following fields are used just for directories */
	char *path;
	size_t path_len;
	char *name;
	size_t name_len;
};

extern struct initfs_file_info *initfs_alloc_inode(void);
extern void initfs_free_inode(struct initfs_file_info *fi);
extern int initfs_iterate(struct inode *next, char *name, struct inode *parent,
    struct dir_ctx *ctx);
extern int initfs_create(struct inode *i_new, struct inode *i_dir, int mode);
extern int initfs_fill_inode(struct inode *node, char *cpio,
    struct cpio_entry *entry);

extern int initfs_destroy_inode(struct inode *inode);

#endif /* SRC_FS_DRIVER_INITFS_INITFS_H_ */
