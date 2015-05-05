/**
 * @file
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <fs/dvfs.h>
#include <kernel/task.h>
#include <kernel/task/resource/file_table.h>

int close(int fd) {
	assert(fd >= 0 && fd < FILE_TABLE_SZ);
	struct file *f = task_fs()->file[fd];

	if (!dvfs_close(f))
		task_fs()->file[fd] = NULL;

	return 0;
}
