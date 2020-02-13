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

struct file_desc {
	struct idesc f_idesc;

	struct dentry *f_dentry;
	struct inode  *f_inode;

	off_t pos;

	const struct file_operations *f_ops;
};

extern off_t file_get_pos(struct file_desc *file);
extern off_t file_set_pos(struct file_desc *file, off_t off);

extern size_t file_get_size(struct file_desc *file);
extern void file_set_size(struct file_desc *file, size_t size);

extern void *file_get_inode_data(struct file_desc *file);

extern struct file_desc *file_desc_from_idesc(struct idesc *idesc);

extern struct file_desc *dvfs_alloc_file(void);
extern int dvfs_destroy_file(struct file_desc *desc);

#endif /* SRC_FS_DVFS_FILE_DESC_H_ */
