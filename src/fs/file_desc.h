/**
 * @file
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */

#ifndef FS_FILE_DESC_H_
#define FS_FILE_DESC_H_

#include <sys/types.h>
#include <fs/idesc.h>

struct node;
struct file_operations;

struct file_desc {
	struct idesc idesc;

	struct node *node;
	int file_flags; //TODO now use only for O_APPEND should move to idesc
	const struct file_operations *ops;
	off_t cursor;
/* TODO this need for system without file_nodes where we want to use uart for
 *  example
 */
	void *file_info; /* customize in each file system */
};

extern off_t file_get_pos(struct file *file);
extern off_t file_set_pos(struct file *file, off_t off);

extern void *file_get_inode_data(struct file *file);

extern struct file_desc *file_desc_create(struct node *node, int __oflag);

extern int file_desc_destroy(struct file_desc *);

extern struct file_desc *file_desc_get(int idx);

#endif /* FS_FILE_DESC_H_ */
