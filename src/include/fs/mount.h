/*
 * @file
 *
 * @date May 28, 2013
 * @author: Anton Bondarev
 */

#ifndef FS_MOUNT_H_
#define FS_MOUNT_H_

#include <lib/libds/dlist.h>

#define MOUNT_DESC_STRINFO_LEN 16

struct path;
struct inode;
struct block_dev;

struct mount_descriptor {
	struct inode *mnt_point; /* Node of the parent FS */
	struct inode *mnt_root;  /* Root node of the mounted FS */
	struct mount_descriptor *mnt_parent;
	struct dlist_head mnt_mounts;
	struct dlist_head mnt_child;
	char mnt_dev[MOUNT_DESC_STRINFO_LEN];
};

extern struct mount_descriptor *mount_table(void);

extern struct mount_descriptor *mount_table_add(struct path *mnt_point_path,
    struct mount_descriptor *mnt_desc, struct inode *root, const char *dev);

extern int mount_table_del(struct mount_descriptor *mdesc);

extern struct mount_descriptor *
mount_table_get_child(struct mount_descriptor *parent, struct inode *mnt_point);

extern struct block_dev *bdev_by_path(const char *source);

#endif /* FS_MOUNT_H_ */
