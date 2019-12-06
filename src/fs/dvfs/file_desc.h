/**
 * @file
 *
 * @date 17 Mar 2015
 * @author Denis Deryugin
 */

#ifndef SRC_FS_DVFS_FILE_DESC_H_
#define SRC_FS_DVFS_FILE_DESC_H_

#include <sys/types.h>
#include <fs/idesc.h>

struct file_operations;
struct inode;
struct dentry;

struct file {
	struct idesc f_idesc;

	struct dentry *f_dentry;
	struct inode  *f_inode;

	off_t pos;

	struct file_operations *f_ops;
};

#endif /* SRC_FS_DVFS_FILE_DESC_H_ */
