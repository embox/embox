/**
 * @file
 *
 * @date 17 Mar 2015
 * @author Denis Deryugin
 */

#ifndef SRC_FS_DVFS_FILE_DESC_H_
#define SRC_FS_DVFS_FILE_DESC_H_

#include <stddef.h>
#include <sys/types.h>

#include <kernel/task/resource/idesc.h>

struct inode;
struct dentry;
struct file_desc;

/**
 * NOTE ON FILE OPEN
 *
 * Basically,  in  regular  file  systems  file  open  driver  function  should
 * just  return  the same  idesc  that  was  passed as second  parameter.  This
 * feature  is  required for device-dependent operations, otherwise just return
 * the second argument.
 */

struct file_operations {
	struct idesc *(
	    *open)(struct inode *node, struct idesc *file_desc, int __oflag);
	int (*close)(struct file_desc *desc);
	size_t (*read)(struct file_desc *desc, void *buf, size_t size);
	size_t (*write)(struct file_desc *desc, void *buf, size_t size);
	int (*ioctl)(struct file_desc *desc, int request, void *data);
};

struct file_desc {
	struct idesc f_idesc;

	struct dentry *f_dentry;
	struct inode *f_inode;

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
