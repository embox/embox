/**
 * @file
 *
 * @date Oct 7, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <mem/sysmalloc.h>

#include <kernel/thread/sync/mutex.h>
#include <kernel/task/thread_key_table.h>
#include <kernel/task/resource/thread_key_table.h>

#include <kernel/thread/thread_local.h>

int thread_local_alloc(struct thread *t, size_t size) {
	void * storage;
	size_t storage_size;

	storage_size = size * sizeof(t->local.storage[0]);
	storage = sysmalloc(storage_size);

	if (NULL == storage) {
		return -ENOMEM;
	}
	memset(storage, 0, storage_size);

	t->local.storage = storage;
	t->local.size = size;

	return ENOERR;
}

int thread_local_free(struct thread *t) {
	sysfree(t->local.storage);

	return ENOERR;
}

void *thread_local_get(struct thread *t, size_t idx) {
	void *res;
	struct thread_key_table *kt;

	assert(t);

	kt = task_resource_thread_key_table(t->task);

	mutex_lock(&kt->mutex);
	{
		if (task_thread_key_exist(t->task, idx)) {
			res = t->local.storage[idx];
		} else {
			res = NULL;
		}
	}
	mutex_unlock(&kt->mutex);

	return res;
}

int thread_local_set(struct thread *t, size_t idx, void *value) {
	int res = ENOERR;
	struct thread_key_table *kt;

	assert(t);

	kt = task_resource_thread_key_table(t->task);

	mutex_lock(&kt->mutex);
	{
		if (task_thread_key_exist(t->task, idx)) {
			t->local.storage[idx] = value;
		} else {
			res = -EINVAL;
		}
	}
	mutex_unlock(&kt->mutex);

	return res;
}
