/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */
#include <stddef.h>
#include <assert.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mmap.h>


TASK_RESOURCE_DEF(task_mmap_desc, struct emmap);

static void task_mmap_init(const struct task *task, void *mmap_space) {
	struct emmap *mmap;

	assert(mmap_space == task_resource_mmap(task));

	mmap = mmap_space;
	assert(mmap != NULL);

	mmap_init(mmap);
}

static int task_mmap_inherit(const struct task *task,
		const struct task *parent) {
	struct emmap *mmap, *parent_mmap;

	mmap = task_resource_mmap(task);
	assert(mmap != NULL);

	parent_mmap = task_resource_mmap(parent);
	assert(parent_mmap != NULL);

	return mmap_inherit(mmap, parent_mmap);
}

void task_mmap_deinit(const struct task *task) {
	struct emmap *mmap;

	mmap = task_resource_mmap(task);
	assert(mmap != NULL);

	mmap_free(mmap);
}

static size_t task_mmap_offset;

static const struct task_resource_desc task_mmap_desc = {
	.init = task_mmap_init,
	.inherit = task_mmap_inherit,
	/* .deinit = task_mmap_deinit, */
	.resource_size = sizeof(struct emmap),
	.resource_offset = &task_mmap_offset
};

struct emmap * task_resource_mmap(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_mmap_offset;
}
