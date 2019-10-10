/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */
#include <errno.h>
#include <stddef.h>
#include <assert.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mapping/marea.h>
#include <mem/mmap.h>
#include <mem/vmem.h>

TASK_RESOURCE_DEF(task_mmap_desc, struct emmap);

static void task_mmap_init(const struct task *task, void *mmap_space) {
	struct emmap *mmap;

	assert(mmap_space == task_resource_mmap(task));

	mmap = mmap_space;
	assert(mmap != NULL);

	dlist_init(&mmap->marea_list);

	vmem_create_context(&mmap->ctx);
}

static int mmap_inherit(struct emmap *mmap, struct emmap *p_mmap) {
	struct marea *marea;
	int err;

	dlist_foreach_entry(marea, &p_mmap->marea_list, mmap_link) {
		if ((err = mmap_place(mmap, marea->start, marea->size, marea->flags))) {
			return err;
		}

		if ((err = vmem_map_region(mmap->ctx,
				vmem_translate(p_mmap->ctx, marea->start, NULL),
				marea->start,
				marea->size,
				marea->flags))) {
				return err;
		}
	}

	return 0;
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

static void task_mmap_deinit(const struct task *task) {
	struct emmap *mmap_self, *mmap_kernel;
	struct marea *marea;

	mmap_self = task_resource_mmap(task);
	assert(mmap_self != NULL);

	mmap_kernel = task_resource_mmap(task_kernel_task());

	/* Need to switch to kernel context as
	 * we are about to purge MMU tables */
	mmu_set_context(mmap_kernel->ctx);

	dlist_foreach_entry(marea, &mmap_self->marea_list, mmap_link) {
		vmem_unmap_region(mmap_self->ctx,
				marea->start,
				marea->size);

		marea_free(marea);
	}

	vmem_free_context(mmap_self->ctx);
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
	return (void *)task->resources + task_mmap_offset;
}
