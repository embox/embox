/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <util/indexator.h>
#include <kernel/task.h>

#include <kernel/task/thread_key_table.h>

void task_thread_key_init(struct task *task) {
	struct thread_key_table *kt;

	kt = &task->key_table;

	index_init(&kt->indexator, 0, THREAD_KEYS_QUANTITY, kt->ind_buff);
	mutex_init(&kt->mutex);
}

int task_thread_key_exist(struct task *task, size_t idx) {
	return index_locked(&task->key_table.indexator, idx);
}

int task_thread_key_create(struct task *task, size_t idx) {
	int res = ENOERR;

	mutex_lock(&task->key_table.mutex);
	{
		if(index_locked(&task->key_table.indexator, idx)) {
			res = -EBUSY;
			goto out;
		}
		index_lock(&task->key_table.indexator, idx);
	}
out:
	mutex_unlock(&task->key_table.mutex);

	return res;
}

int task_thread_key_destroy(struct task *task, size_t idx) {
	int res;

	mutex_lock(&task->key_table.mutex);
	{
		if(!index_locked(&task->key_table.indexator, idx)) {
			res = -EALREADY;
			goto out;
		}
		index_unlock(&task->key_table.indexator, idx);
	}
out:
	mutex_lock(&task->key_table.mutex);
	return res;
}
