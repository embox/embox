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

#include <util/dlist.h>



TASK_RESOURCE_DEF(task_heap_desc, struct task_heap);

extern int heap_init(const struct task *task);
extern int heap_fini(const struct task *task);


struct task_heap *task_heap_get(const struct task *task);

static void task_heap_init(const struct task *task, void *task_heap) {
	heap_init(task);
}

static int task_heap_inherit(const struct task *task, const struct task *parent) {
	heap_init(task);
	return 0;
}

static void task_heap_deinit(const struct task *task) {
	heap_fini(task);
}

static size_t task_heap_offset;

static const struct task_resource_desc task_heap_desc = {
	.init = task_heap_init,
	.inherit = task_heap_inherit,
	.deinit = task_heap_deinit,
	.resource_size = sizeof(struct task_heap),
	.resource_offset = &task_heap_offset
};

struct task_heap *task_heap_get(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_heap_offset;
}



