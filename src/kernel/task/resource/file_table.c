/**
 * @filoe
 * @brief
 *
 * @date Mar 28 2015
 * @author Denis Deryugin
 */

#include <fs/dvfs.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/file_table.h>

TASK_RESOURCE_DEF(file_table, struct file_table);

static void file_table_init(const struct task *task, void *file_table) {
	struct file_table *fs = file_table;
	memset(fs, 0, sizeof(struct file_table));

	fs->root = fs->pwd = dvfs_root();
}

static void file_table_deinit(const struct task *task) {
	/* TODO destroy file descriptors */
}

static size_t file_table_offset;

static const struct task_resource_desc file_table = {
	.init   = file_table_init,
	.deinit = file_table_deinit,
	.resource_size = sizeof(struct file_table),
	.resource_offset = &file_table_offset
};

struct file_table *task_resource_file_table(const struct task *task) {
	return (void*)task->resources + file_table_offset;
}

int file_table_dupfd(int fd_old, int fd_new) {
	if (fd_new < 0 || fd_new >= FILE_TABLE_SZ)
		return -1;

	if (task_fs()->file[fd_new] != NULL)
		return -1;

	task_fs()->file[fd_new] = task_fs()->file[fd_old];

	return 0;
}

int file_table_add(struct file_table *ft, struct file *f) {
	int i;

	assert(ft);
	assert(f);

	for (i = 0; i < FILE_TABLE_SZ; i++) {
		if (ft->file[i] == NULL) {
			ft->file[i] = f;
			return 0;
		}
	}

	return -ENOMEM;
}
