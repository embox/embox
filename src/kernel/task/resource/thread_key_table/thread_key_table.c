/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */

#include <assert.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/thread_key_table.h>
#include <kernel/thread/thread_local.h>
#include <stddef.h>
#include <lib/libds/indexator.h>

TASK_RESOURCE_DEF(task_thread_key_table_desc,
		struct thread_key_table);

static void task_thread_key_table_init(const struct task *task,
		void *thread_key_table_space) {
	struct thread_key_table *kt;

	assert(thread_key_table_space
			== task_resource_thread_key_table(task));

	kt = thread_key_table_space;

	index_init(&kt->indexator, 0, MODOPS_THREAD_KEY_QUANTITY,
			kt->index_buffer);
	mutex_init(&kt->mutex);
}

static size_t task_thread_key_table_offset;

static const struct task_resource_desc task_thread_key_table_desc = {
	.init = task_thread_key_table_init,
	.resource_size = sizeof(struct thread_key_table),
	.resource_offset = &task_thread_key_table_offset
};

struct thread_key_table * task_resource_thread_key_table(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_thread_key_table_offset;
}
