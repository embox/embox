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

static struct dlist_head *task_self_mspace(void) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task_self());
	return &task_heap->mm;
}

void *memalign(size_t boundary, size_t size) {
	return mspace_memalign(boundary, size, task_self_mspace());
}

void *malloc(size_t size) {
	void *ptr;

	if (size == 0) {
		return NULL;
	}

	ptr = mspace_malloc(size, task_self_mspace());

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
	mspace_free(ptr, task_self_mspace());
}

void *realloc(void *ptr, size_t size) {
	if (size == 0 && ptr != NULL) {
		mspace_free(ptr, task_self_mspace());
		return NULL; /* ok */
	}
	return mspace_realloc(ptr, size, task_self_mspace());
}

void *calloc(size_t nmemb, size_t size) {
	if (nmemb == 0 || size == 0)
		return NULL; /* ok */
	return mspace_calloc(nmemb, size, task_self_mspace());
}
