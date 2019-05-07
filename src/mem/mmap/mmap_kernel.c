/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */

#include <util/log.h>

#include <stddef.h>

#include <util/dlist.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <kernel/panic.h>

#include <mem/vmem.h>

#include <mem/mapping/mmap.h>
#include <mem/mapping/marea.h>

static struct emmap early_emmap = {
	NULL,
	0,
	DLIST_INIT(early_emmap.marea_list)
};

extern int vmem_map_kernel(void);

int mmap_kernel_init(void) {
	int ret;
	struct marea *marea;
	struct emmap *emmap;

	emmap = task_resource_mmap(task_kernel_task());

	dlist_foreach_entry(marea, &early_emmap.marea_list, mmap_link) {
		dlist_del_init(&marea->mmap_link);
		dlist_add_next(&marea->mmap_link, &emmap->marea_list);
		log_debug("add to %p start %p end %p",marea, marea->start, marea->end);
	}
	ret = vmem_map_kernel();
	assert(ret == 0);

	early_emmap.ctx = -1;

	if (mmap_mapping(emmap)) {
		panic("Memory map initialization error!");
	}

	mmu_set_context(emmap->ctx);

	vmem_on();

	return 0;
}

int mmap_kernel_inited(void) {
	if (-1 == early_emmap.ctx) {
		return 1;
	}

	return 0;
}

struct emmap *mmap_early_emmap(void) {
	return &early_emmap;
}
