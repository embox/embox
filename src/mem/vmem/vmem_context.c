/**
 * @file
 *
 * @date Jul 31, 2014
 * @author: Anton Bondarev
 */
#include <errno.h>

#include <hal/mmu.h>
#include <mem/vmem.h>
#include <mem/mmap.h>
#include <mem/vmem/vmem_alloc.h>

#include <kernel/sched/sched_lock.h>

#include <kernel/task/resource/mmap.h>

int vmem_create_context(mmu_ctx_t *ctx) {
	uintptr_t *pgd = vmem_alloc_table(0);

	if (!pgd) {
		return -ENOMEM;
	}

	*ctx = mmu_create_context(pgd);

	return ENOERR;
}

mmu_ctx_t vmem_current_context(void) {
	struct emmap *emmap;

	emmap = task_self_resource_mmap();

	return emmap->ctx;
}

static void vmem_free_table_level(int lvl, uintptr_t *tbl) {
	if (lvl == MMU_LAST_LEVEL) {
		vmem_free_table(lvl, tbl);
		return;
	}
	for (int idx = 0; idx < MMU_ENTRIES(lvl); idx++) {
		if (mmu_present(lvl, tbl + idx)) {
			vmem_free_table_level(lvl + 1, mmu_get(lvl, (tbl + idx)));
		}
	}
	vmem_free_table(lvl, tbl);
}

void vmem_free_context(mmu_ctx_t ctx) {
	vmem_free_table_level(0, mmu_get_root(ctx));
}
