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
}

int task_thread_key_exist(struct task *task, size_t idx) {
	return index_locked(&task->key_table.indexator, idx);
}

int task_thread_key_create(struct task *task, size_t idx) {
	if(index_locked(&task->key_table.indexator, idx)) {
		return -EBUSY;
	}
	index_lock(&task->key_table.indexator, idx);
	return ENOERR;
}

int task_thread_key_destroy(struct task *task, size_t idx) {
	if(!index_locked(&task->key_table.indexator, idx)) {
		return -EALREADY;
	}
	index_unlock(&task->key_table.indexator, idx);
	return ENOERR;
}
