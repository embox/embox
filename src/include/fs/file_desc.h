/**
 * @file
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */

#ifndef FS_FILE_DESC_H_
#define FS_FILE_DESC_H_

#include <lib/list.h>

struct node;

struct file_desc {
	struct node *node;
	struct file_operations *ops;
	size_t cursor;
};

extern struct file_desc *file_desc_alloc(void);

extern void file_desc_free(struct file_desc *desc);

#endif /* FS_FILE_DESC_H_ */
