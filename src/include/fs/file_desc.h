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
#include <kernel/task/io_sync.h>
#include <fs/idesc.h>

struct node;
struct kfile_operations;

struct file_desc {
	struct idesc idesc;

	struct node *node;
	int flags;
	const struct kfile_operations *ops;
	size_t cursor;
/* TODO this need for system without file_nodes where we want to use uart for
 *  example
 */
	void *file_info; /* customize in each file system */

	struct io_sync ios;
};

extern struct file_desc *file_desc_alloc(void);

extern void file_desc_free(struct file_desc *desc);
struct node;
extern struct file_desc *file_desc_create(struct node *node, int __oflag, mode_t imode);

extern int file_desc_desctroy(struct file_desc *);
extern int file_desc_perm_check(struct file_desc *);

#endif /* FS_FILE_DESC_H_ */
