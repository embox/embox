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
#include <kernel/task/resource/idesc_table.h>

int open(const char *path, int __oflag, ...) {
	struct file *file;
	struct idesc_table *it;
	int res;

	file = dvfs_alloc_file();
	if (file == NULL)
		return SET_ERRNO(ENOMEM);

	res = dvfs_open(path, file, __oflag);
	if (res) {
		dvfs_destroy_file(file);
		return SET_ERRNO(-res);
	}

	it = task_resource_idesc_table(task_self());
	res = idesc_table_add(it, (struct idesc *) file, 0);

	if (res >= 0)
		return res;

	dvfs_destroy_file(file);
	return SET_ERRNO(ENOMEM);
}

