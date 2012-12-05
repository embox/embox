/**
 * @file
 * @brief Kernel file abstraction
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#ifndef KERNEL_FILE_H_
#define KERNEL_FILE_H_

#include <types.h>
#include <stdarg.h>


struct node;
struct file_desc;

typedef int    (*kfile_op_open_ft)(struct node *node, struct file_desc *file_desc, int flags);
typedef int    (*kfile_op_close_ft)(struct file_desc *desc);
typedef size_t (*kfile_op_read_ft)(struct file_desc *desc, void *buf, size_t size);
typedef size_t (*kfile_op_write_ft)(struct file_desc *desc, void *buf, size_t size);
typedef int    (*kfile_op_ioctl_ft)(struct file_desc *desc, int request, va_list args);

typedef struct kfile_operations {
	kfile_op_open_ft  open;
	kfile_op_close_ft close;
	kfile_op_read_ft  read;
	kfile_op_write_ft write;
	kfile_op_ioctl_ft ioctl;
} kfile_operations_t;

#endif /* KERNEL_FILE_H_ */
