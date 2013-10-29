/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.10.2012
 */
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#include <kernel/task/idx.h>
#include <util/array.h>
#include <fs/posix.h>

#include <fs/kfile.h>


int open(const char *path, int __oflag, ...) {
	struct file_desc *kfile;
	va_list args;
	mode_t mode;
	int rc;
	struct stat st;

	va_start(args, __oflag);
	mode = va_arg(args, mode_t);
	va_end(args);

	if (ENOERR != (rc = stat(path, &st))) {
		return rc;
	}
	switch (st.st_mode & S_IFMT) {
	case S_IFDIR:
		if (__oflag & O_DIRECTORY) {
			/*
			DIR *dentry;
			if (NULL == (dentry = opendir(path)) {
				return -EACCES;
			}


			rc = task_self_idx_alloc(&task_idx_ops_file, dentry, &kfile->ios);
			*/
			return -ENOSUPP;
		}
		return -ENOTDIR;
	case S_IFREG:
	case S_IFBLK:
	case S_IFCHR:
	case S_IFIFO: {
			kfile = kopen(path, __oflag, mode);
			if (NULL == kfile) {
				return -errno;
			}

			rc = task_self_idx_alloc(&task_idx_ops_file, kfile, &kfile->ios);
			break;
		}
	}

	return rc;
}
