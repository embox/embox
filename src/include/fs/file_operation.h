/**
 * @file
 * @brief Kernel file abstraction
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#ifndef FS_FILE_OPERATION_H_
#define FS_FILE_OPERATION_H_


#include <stdarg.h>
#include <stddef.h>

struct node;
struct file_desc;

struct kfile_operations {
	int    (*open)(struct node *node, struct file_desc *file_desc, int flags);
	int    (*close)(struct file_desc *desc);
	size_t (*read)(struct file_desc *desc, void *buf, size_t size);
	size_t (*write)(struct file_desc *desc, void *buf, size_t size);
	int    (*ioctl)(struct file_desc *desc, int request, ...);
};

#endif /* FS_FILE_OPERATION_H_ */
