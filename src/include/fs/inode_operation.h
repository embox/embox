/**
 * @file
 *
 * @date Dec 25, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_INCLUDE_FS_INODE_OPERATION_H_
#define SRC_INCLUDE_FS_INODE_OPERATION_H_

#include <stddef.h>

struct inode;
struct dentry;
struct dir_ctx;

struct inode_operations {
	int           (*create)(struct inode *i_new, struct inode *i_dir, int mode);
	struct inode *(*lookup)(char const *name, struct inode const *dir);
	int           (*remove)(struct inode *inode);
	int           (*mkdir)(struct dentry *d_new, struct dentry *d_parent);
	int           (*rmdir)(struct dentry *dir);
	int           (*truncate)(struct inode *inode, size_t len);
	int           (*pathname)(struct inode *inode, char *buf, int flags);
	int           (*iterate)(struct inode *next_inode, char *name_buf,
			struct inode *parent, struct dir_ctx *ctx);
	int           (*rename)(struct inode *node, struct inode *new_parent, const char *new_name);
	int           (*getxattr)(struct inode *node, const char *name, char *value, size_t size);
	int           (*setxattr)(struct inode *node, const char *name, const char *value, size_t size, int flags);
};

#endif /* SRC_INCLUDE_FS_INODE_OPERATION_H_ */
