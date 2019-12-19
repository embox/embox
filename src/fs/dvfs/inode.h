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
	int    i_no;
	int    start_pos; /* location on disk */
	size_t length;
	int    flags;
	uid_t  i_owner_id;
	gid_t  i_group_id;
	mode_t i_mode;

	struct dentry *i_dentry;
	struct super_block *i_sb;
	struct inode_operations *i_ops;

	void *i_data;
};

#endif /* SRC_FS_DVFS_INODE_H_ */
