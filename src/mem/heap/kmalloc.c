/**
 * @file
 * @brief

 * @date 07.04.2014
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <unistd.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/task_heap.h>

#include "mspace_malloc.h"

static struct dlist_head *kernel_task_mspace(void) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task_kernel_task());
	return &task_heap->mm;
}

void *kmemalign(size_t boundary, size_t size) {
	return mspace_memalign(boundary, size, kernel_task_mspace());
}

void *kmalloc(size_t size) {
	return mspace_malloc(size, kernel_task_mspace());
}

void kfree(void *ptr) {
	mspace_free(ptr, kernel_task_mspace());
}

void *krealloc(void *ptr, size_t size) {
	return mspace_realloc(ptr, size, kernel_task_mspace());
}

void *kcalloc(size_t nmemb, size_t size) {
	return mspace_calloc(nmemb, size, kernel_task_mspace());
}
