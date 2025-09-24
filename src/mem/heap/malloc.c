/**
 * @file
 * @brief

 * @date 07.04.2014
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <util/err.h>
#include <unistd.h>
#include <lib/libds/dlist.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/task_heap.h>
#include <kernel/printk.h>
#include <mem/heap/mspace_malloc.h>

static struct dlist_head *task_self_mspace(void) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task_self());
	return &task_heap->mm;
}

static struct dlist_head *kernel_task_mspace(void) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task_kernel_task());
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
		SET_ERRNO(ENOMEM);
	}

	return ptr;
}

void free(void *ptr) {
	if (ptr == NULL)
		return;
	/* XXX this workaround for such situation:
	 * module ConstructionGlobal invokes constructors inside kernel task for all applications,
	 * and call malloc. After a while Qt application call realloc() on some memory previously
	 * allocated by malloc() in kernel task. */
	if (0 > mspace_free(ptr, task_self_mspace())) {
		printk("***** free: pointer is not in current task, try free in kernel task...\n");
		if (0 > mspace_free(ptr, kernel_task_mspace())) {
			assert(0);
		}
	}
}

void *realloc(void *ptr, size_t size) {
	void *ret;

	if (size == 0 && ptr != NULL) {
		mspace_free(ptr, task_self_mspace());
		return NULL; /* ok */
	}
	if (ptr == NULL) {
		return malloc(size);
	}
	/* XXX same as in free() above */
	if (0 > ptr2err(ret = mspace_realloc(ptr, size, task_self_mspace()))) {
		printk("***** realloc: pointer is not in current task, try realloc in kernel task...\n");
		if (0 > ptr2err(ret = mspace_realloc(ptr, size, kernel_task_mspace()))) {
			assert(0);
		}
	}

	return ret;
}

void *calloc(size_t nmemb, size_t size) {
	if (nmemb == 0 || size == 0)
		return NULL; /* ok */
	return mspace_calloc(nmemb, size, task_self_mspace());
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
	*memptr = memalign(alignment, size);
	return 0;
}

