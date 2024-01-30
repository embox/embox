/**
 * @file
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */

#ifndef FS_FILE_DESC_H_
#define FS_FILE_DESC_H_

#include <stddef.h>
#include <sys/types.h>

#include <kernel/task/resource/idesc.h>

struct inode;
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

	struct inode *f_inode;
	int file_flags; //TODO now use only for O_APPEND should move to idesc
	const struct file_operations *f_ops;
	off_t f_pos;
	/* TODO this need for system without file_nodes where we want to use uart for
 *  example
 */
	void *file_info; /* customize in each file system */
};

extern struct file_desc *file_desc_from_idesc(struct idesc *idesc);

extern void file_desc_set_file_info(struct file_desc *file, void *fi);

extern off_t file_get_pos(struct file_desc *file);
extern off_t file_set_pos(struct file_desc *file, off_t off);

extern size_t file_get_size(struct file_desc *file);
extern void file_set_size(struct file_desc *file, size_t size);

extern void *file_get_inode_data(struct file_desc *file);

extern struct file_desc *file_desc_create(struct inode *node, int __oflag);

extern int file_desc_destroy(struct file_desc *);

extern struct file_desc *file_desc_get(int idx);

#endif /* FS_FILE_DESC_H_ */
