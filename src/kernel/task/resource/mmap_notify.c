/**
 * @file
 * @brief
 *
 * @date 11.03.14
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <hal/mmu.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/mmap.h>
#include <kernel/thread.h>
#include <mem/mmap.h>
#include <stddef.h>

TASK_RESOURCE_NOTIFY(task_mmap_notify);

static int task_mmap_notify(struct thread *prev, struct thread *next) {
	struct emmap *mmap;

	assert(prev != NULL);
	assert(next != NULL);

	mmap = task_resource_mmap(next->task);
	assert(mmap != NULL);

	mmu_set_context(mmap->ctx);

	return 0;
}
