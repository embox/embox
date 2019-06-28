/**
 * @file mmap_trivial.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 22.03.2019
 */

#include <stddef.h>
#include <assert.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/mmap.h>
#include <mem/vmem.h>
#include <mem/mmap.h>

TASK_RESOURCE_DEF(task_mmap_desc, struct emmap);

static void task_mmap_init(const struct task *task, void *mmap_space) {
	struct emmap *mmap = mmap_space;
	assert(mmap != NULL);

	if (task == task_kernel_task()) {
		dlist_init(&mmap->marea_list);
		vmem_create_context(&mmap->ctx);
	}
}

static int task_mmap_inherit(const struct task *task,
		const struct task *parent) {
	struct emmap *mmap, *parent_mmap;

	mmap = task_resource_mmap(task);
	assert(mmap != NULL);

	parent_mmap = task_resource_mmap(parent);
	assert(parent_mmap != NULL);

	mmap->ctx = parent_mmap->ctx;

	return 0;
}

void task_mmap_deinit(const struct task *task) {
	/* Do nothing */
}

static size_t task_mmap_offset;

static const struct task_resource_desc task_mmap_desc = {
	.init = task_mmap_init,
	.inherit = task_mmap_inherit,
	.deinit = task_mmap_deinit,
	.resource_size = sizeof(struct emmap),
	.resource_offset = &task_mmap_offset
};

struct emmap * task_resource_mmap(const struct task *task) {
	assert(task != NULL);

	if (task == task_kernel_task()) {
		return (void *)task->resources + task_mmap_offset;
	} else {
		return task_resource_mmap(task_kernel_task());
	}
}
