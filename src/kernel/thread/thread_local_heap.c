/**
 * @file
 *
 * @date Oct 7, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <stdlib.h>
#include <kernel/thread.h>
#include <kernel/task.h>

#include <kernel/task/thread_key_table.h>

#include <kernel/thread/thread_local.h>

int thread_local_alloc(struct thread *t, size_t size) {
	void * storage;

	storage = malloc(size * sizeof(void *));

	if (NULL == storage) {
		return -ENOMEM;
	}

	t->local.storage = storage;
	t->local.size = size;

	return ENOERR;
}


int thread_local_free(struct thread *t) {
	free(*t->local.storage);

	return ENOERR;
}

void *thread_local_get(struct thread *t, size_t idx) {
	void *res;

	assert(t);

	mutex_lock(&t->task->key_table.mutex);
	{
		if(task_thread_key_exist(t->task, idx)) {
			res = t->local.storage[idx];
		} else {
			res = NULL;
		}
	}
	mutex_unlock(&t->task->key_table.mutex);

	return res;
}

int thread_local_set(struct thread *t, size_t idx, void *value) {
	int res = ENOERR;

	assert(t);

	mutex_lock(&t->task->key_table.mutex);
	{
		if(task_thread_key_exist(t->task, idx)) {
			t->local.storage[idx] = value;
		} else {
			res = -EINVAL;
		}
	}
	mutex_unlock(&t->task->key_table.mutex);

	return res;
}
