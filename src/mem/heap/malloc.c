/**
 * @file
 * @brief

 * @date 07.04.2014
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <unistd.h>
#include <util/dlist.h>

#include <kernel/task.h>
#include <kernel/task/resource/task_heap.h>

#include "mspace_malloc.h"

static struct dlist_head *task_to_mspace(struct task *task) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task);
	return &task_heap->mm;
}

void *memalign(size_t boundary, size_t size) {
	return mspace_memalign(boundary, size, task_to_mspace(task_self()));
}

void *malloc(size_t size) {
	void *ptr;

	if (size == 0) {
		return NULL;
	}

	ptr = mspace_malloc(size, task_to_mspace(task_self()));

	if (ptr == NULL) {
		if (size == 0) {
			return NULL;
		} else {
			SET_ERRNO(ENOMEM);
			return NULL;
		}
	}

	return ptr;
}

void free(void *ptr) {
	if (ptr == NULL)
		return;
	mspace_free(ptr, task_to_mspace(task_self()));
}

void *realloc(void *ptr, size_t size) {
	if (size == 0 && ptr != NULL) {
		mspace_free(ptr, task_to_mspace(task_self()));
		return NULL; /* ok */
	}
	return mspace_realloc(ptr, size, task_to_mspace(task_self()));
}

void *calloc(size_t nmemb, size_t size) {
	if (nmemb == 0 || size == 0)
		return NULL; /* ok */
	return mspace_calloc(nmemb, size, task_to_mspace(task_self()));
}
