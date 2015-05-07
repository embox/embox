/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    29 Mar 2015
 */

#include <fcntl.h>
#include <fs/dvfs.h>
#include <kernel/task.h>
#include <kernel/task/resource/file_table.h>

int open(const char *path, int __oflag, ...) {
	struct file *file;
	struct file_table *ft;
	int res;

	file = dvfs_alloc_file();
	if (file == NULL)
		return SET_ERRNO(ENOMEM);

	res = dvfs_open(path, file, __oflag);
	if (res) {
		dvfs_destroy_file(file);
		return SET_ERRNO(-res);
	}

	ft = task_fs();
	for (res = 0; res < FILE_TABLE_SZ; res++)
		if (ft->file[res] == NULL) {
			ft->file[res] = file;
			return res;
		}

	dvfs_destroy_file(file);
	return SET_ERRNO(ENOMEM);
}

