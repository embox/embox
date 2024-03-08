/**
 * @file
 * @brief

 * @date 07.04.2014
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <unistd.h>

#include <util/err.h>

#include <kernel/task.h>
#include <kernel/printk.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/task_heap.h>
#include <mem/sysmalloc.h>

#include "mspace_malloc.h"

static struct dlist_head *kernel_task_mspace(void) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task_kernel_task());
	return &task_heap->mm;
}

void *sysmemalign(size_t boundary, size_t size) {
	return mspace_memalign(boundary, size, kernel_task_mspace());
}

void *sysmalloc(size_t size) {
	return mspace_malloc(size, kernel_task_mspace());
}

void sysfree(void *ptr) {
	if (!ptr) {
		printk(">>>%s: NULL pointer\n", __func__);
		return;
	}
	mspace_free(ptr, kernel_task_mspace());
}

void *sysrealloc(void *ptr, size_t size) {
	void *ret;

	ret = mspace_realloc(ptr, size, kernel_task_mspace());
	if (0 > ptr2err(ret)) {
		return NULL;
	}

	return ret;
}

void *syscalloc(size_t nmemb, size_t size) {
	return mspace_calloc(nmemb, size, kernel_task_mspace());
}
