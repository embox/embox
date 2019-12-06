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

extern off_t file_get_pos(struct file *file);
extern off_t file_set_pos(struct file *file, off_t off);

extern void *file_get_inode_data(struct file *file);

extern struct file *dvfs_alloc_file(void);
extern int dvfs_destroy_file(struct file *desc);

#endif /* SRC_FS_DVFS_FILE_DESC_H_ */
