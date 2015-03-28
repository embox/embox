/**
 * @filoe
 * @brief
 *
 * @date Mar 28 2015
 * @author Denis Deryugin
 */

#include <kernel/task/resource.h>
#include <kernel/task/resource/file_table.h>

TASK_RESOURCE_DEF(file_table, struct file_table);

static void file_table_init(const struct task *task, void *file_table) {
	memset(file_table, 0, sizeof(struct file_table));
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
