/**
 * @file
 * @brief
 *
 * @date 28.02.14
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/thread_key_table.h>
#include <kernel/task/resource/thread_key_table.h>
#include <kernel/thread/sync/mutex.h>
#include <lib/libds/indexator.h>

int task_thread_key_exist(struct task *task, size_t idx) {
	assert(task);
	return index_locked(&task_resource_thread_key_table(task)->indexator, idx);
}

int task_thread_key_create(struct task *task, size_t *idx) {
	int res = ENOERR;
	struct thread_key_table *kt;

	kt = task_resource_thread_key_table(task);

	mutex_lock(&kt->mutex);
	{
		*idx = index_find(&kt->indexator, INDEX_MIN);
		if (*idx == INDEX_NONE) {
			res = -EBUSY;
			goto out;
		}
		index_lock(&kt->indexator, *idx);
	}
out:
	mutex_unlock(&kt->mutex);

	return res;
}

int task_thread_key_destroy(struct task *task, size_t idx) {
	int res = ENOERR;
	struct thread_key_table *kt;

	kt = task_resource_thread_key_table(task);

	mutex_lock(&kt->mutex);
	{
		if(!index_locked(&kt->indexator, idx)) {
			res = -EALREADY;
			goto out;
		}
		index_unlock(&kt->indexator, idx);
	}
out:
	mutex_unlock(&kt->mutex);
	return res;
}
