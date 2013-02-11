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

#define FDESK_FLAG_EXEC    (0x1 << 0) /* must equal to S_IXOTH */
#define FDESK_FLAG_WRITE   (0x1 << 1) /* must equal to S_IWOTH */
#define FDESK_FLAG_READ    (0x1 << 2) /* must equal to S_IROTH */
#define FDESK_FLAG_APPEND  (0x1 << 3)

struct file_desc {
	struct node *node;
	int flags;
	const struct kfile_operations *ops;
	size_t cursor;
	void *file_info; /* customize in each file system */
};

extern struct file_desc * file_desc_alloc(void);

extern void file_desc_free(struct file_desc *desc);

#if FDESK_FLAG_READ  != S_IROTH || \
	FDESK_FLAG_WRITE != S_IWOTH || \
	FDESK_FLAG_EXEC  != S_IXOTH
# error "Please arrange FDESK_FLAG_XXX to match S_I[RWX]OTH values"
#endif

#endif /* FS_FILE_DESC_H_ */
