/**
 * @file
 *
 * @date 11.10.10
 * @author Anton Bondarev
 */

#ifndef FS_FILE_H_
#define FS_FILE_H_

#include <types.h>
#include <stdio.h>
#include <kernel/file.h>

typedef struct file {
	const char                    *f_path;
	const struct file_operations  *f_op;
	spinlock_t                     f_lock;
	unsigned int                   f_mode;
	unsigned int                   f_pos;
} file_t;


#endif /* FS_FILE_H_ */
