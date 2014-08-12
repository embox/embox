/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */

#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>

#include <module/embox/mem/mmap_mmu.h>

static struct emmap early_emmap = {
	NULL,
	NULL,
	NULL,
	0,
	DLIST_INIT(early_emmap.marea_list)
};

int mmap_kernel_init(void) {
	early_emmap.ctx = -1;
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
