/**
 * @file
 *
 * @date Dec 25, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_INCLUDE_FS_INODE_OPERATION_H_
#define SRC_INCLUDE_FS_INODE_OPERATION_H_

#include <stddef.h>
#include <sys/types.h>

struct inode;
struct dentry;
struct dir_ctx;

struct inode_operations {
	struct inode *(*ino_lookup)(const char *name, const struct inode *dir);
	int (*ino_create)(struct inode *i_new, struct inode *i_dir, int mode);
	int (*ino_remove)(struct inode *inode);
	int (*ino_mkdir)(struct dentry *d_new, struct dentry *d_parent);
	int (*ino_rmdir)(struct dentry *dir);
	int (*ino_truncate)(struct inode *inode, off_t len);
	int (*ino_pathname)(struct inode *inode, char *buf, int flags);
	int (*ino_iterate)(struct inode *next_inode, char *name_buf,
	    struct inode *parent, struct dir_ctx *ctx);
	int (*ino_rename)(struct inode *node, struct inode *new_parent,
	    const char *new_name);

	int (*ino_getxattr)(struct inode *node, const char *name, char *value,
	    size_t size);
	int (*ino_setxattr)(struct inode *node, const char *name, const char *value,
	    size_t size, int flags);
	int (*ino_listxattr)(struct inode *node, char *list, size_t len);
};

#endif /* SRC_INCLUDE_FS_INODE_OPERATION_H_ */
