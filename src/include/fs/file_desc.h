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
#include <fs/idesc.h>


struct node;
struct kfile_operations;

struct file_desc {
	struct idesc idesc;

	struct node *node;
	int file_flags; //TODO now use only for O_APPEND schould move to idesc
	const struct kfile_operations *ops;
	size_t cursor;
/* TODO this need for system without file_nodes where we want to use uart for
 *  example
 */
	void *file_info; /* customize in each file system */
};


struct node;
extern struct file_desc *file_desc_create(struct node *node, int __oflag);

extern int file_desc_destroy(struct file_desc *);

extern struct file_desc *file_desc_get(int idx);

#endif /* FS_FILE_DESC_H_ */
