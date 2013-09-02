/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.10.2012
 */

#include <fcntl.h>
#include <kernel/task/idx.h>
#include <util/array.h>
#include <fs/posix.h>

#include <fs/kfile.h>

int open(const char *path, int __oflag, ...) {
	struct file_desc *kfile;
	va_list args;
	mode_t mode;
	int rc;

	va_start(args, __oflag);
	mode = va_arg(args, mode_t);
	va_end(args);

	kfile = kopen(path, __oflag, mode);

	if (NULL == kfile) {
		return -1;
	}

	rc = task_self_idx_alloc(&task_idx_ops_file, kfile, &kfile->ios);
	DPRINTF(("open(%s, %d ...) = %d\n", path, __oflag, rc));
	return rc;
}
