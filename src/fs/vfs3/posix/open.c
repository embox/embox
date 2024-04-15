/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/task.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_table.h>
#include <util/err.h>
#include <vfs/core.h>

int open(const char *path, int oflag, ...) {
	struct idesc *idesc;
	va_list args;
	mode_t mode;
	int err;
	int fd;

	(void)mode;
	if (oflag & O_CREAT) {
		va_start(args, oflag);
		mode = va_arg(args, mode_t);
		mode &= ~umask(0);
		va_end(args);
	}

	vfs_inode_ops_lock();
	idesc = vfs_path_open(path, oflag, mode);
	vfs_inode_ops_unlock();

	if ((err = ptr2err(idesc))) {
		return SET_ERRNO(-err);
	}

	fd = idesc_table_add(task_resource_idesc_table(task_self()), idesc, 0);
	if (fd < 0) {
		idesc->idesc_ops->close(idesc);
		return SET_ERRNO(EMFILE);
	}

	return fd;
}
