/**
 * @file
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */

#ifndef FS_FILE_DESC_H_
#define FS_FILE_DESC_H_

#include <fs/node.h>
#include <fs/file_operation.h>
#include <stddef.h>

struct node;
struct kfile_operations;

#define FDESK_FLAG_WRITE	    (1 << 0)
#define FDESK_FLAG_READ		    (1 << 1)
#define FDESK_FLAG_APPEND	    (1 << 2)

struct file_desc {
	struct node *node;
	int flags;
	const struct kfile_operations *ops;
	size_t cursor;
	void *file_info; /* customize in each file system */
};

extern struct file_desc * file_desc_alloc(void);

extern void file_desc_free(struct file_desc *desc);

#endif /* FS_FILE_DESC_H_ */
