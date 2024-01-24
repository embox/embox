/**
 * @file
 * @brief
 *
 * @date Mar 31, 2014
 * @author Anton Bondarev
 */

#include <assert.h>
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/task_heap.h>

#include <lib/libds/dlist.h>

TASK_RESOURCE_DEF(task_heap_desc, struct task_heap);

extern int mspace_init(struct dlist_head *mspace);
extern int mspace_fini(struct dlist_head *mspace);

static void task_heap_init(const struct task *task, void *task_heap) {
	mspace_init(&task_heap_get(task)->mm);
}

static void task_heap_deinit(const struct task *task) {
	mspace_fini(&task_heap_get(task)->mm);
}

static size_t task_heap_offset;

static const struct task_resource_desc task_heap_desc = {
	.init = task_heap_init,
	.deinit = task_heap_deinit,
	.resource_size = sizeof(struct task_heap),
	.resource_offset = &task_heap_offset
};

struct task_heap *task_heap_get(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_heap_offset;
}
