/*
 * inode.h
 *
 *  Created on: Dec 19, 2019
 *      Author: anton
 */

#ifndef SRC_FS_DVFS_INODE_H_
#define SRC_FS_DVFS_INODE_H_

#include <stddef.h>
#include <sys/types.h>

struct dentry;
struct super_block;
struct inode_operations;

struct inode {
	int      i_no;
	size_t   i_size;
	unsigned int  i_ctime; /* time of last status change */
	unsigned int  i_mtime;

	uid_t  i_owner_id;
	gid_t  i_group_id;
	mode_t i_mode;

	struct dentry *i_dentry;
	struct super_block *i_sb;
	struct inode_operations *i_ops;

	void *i_data;
};

extern void *inode_priv(const struct inode *node);
extern void inode_priv_set(struct inode *node, void *priv);
extern size_t inode_size(const struct inode *node);
extern void inode_size_set(struct inode *node, size_t sz);
extern unsigned inode_ctime(const struct inode *node);
extern void inode_ctime_set(struct inode *node, unsigned ctime);
extern unsigned inode_mtime(const struct inode *node);
extern void inode_mtime_set(struct inode *node, unsigned mtime);
extern char *inode_name(struct inode *node);

extern struct inode  *dvfs_alloc_inode(struct super_block *sb);
#define inode_new     dvfs_alloc_inode

extern int            dvfs_destroy_inode(struct inode *inode);
#define inode_del     dvfs_destroy_inode

#endif /* SRC_FS_DVFS_INODE_H_ */
