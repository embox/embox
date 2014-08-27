/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */

#include <stddef.h>

#include <util/dlist.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>

#include <module/embox/mem/mmap_mmu.h>

static struct emmap early_emmap = {
	NULL,
	NULL,
	NULL,
	0,
	DLIST_INIT(early_emmap.marea_list)
};

extern int mmap_mapping(struct emmap *emmap);
int mmap_kernel_init(void) {
	struct marea *marea;
	struct emmap *emmap;

	emmap = task_resource_mmap(task_kernel_task());

	dlist_foreach_entry(marea, &early_emmap.marea_list, mmap_link) {
		dlist_del_init(&marea->mmap_link);
		dlist_add_next(&marea->mmap_link, &emmap->marea_list);
	}
	early_emmap.ctx = -1;

	mmap_mapping(emmap);

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
