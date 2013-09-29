/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include <hal/mmu.h>
#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>
#include <kernel/sched/sched_lock.h>

#include "../kernel/task/common.h"
#include <stdint.h>
#include <kernel/printk.h>

/* Section pointers. */
extern char _text_vma, _rodata_vma, _bss_vma, _data_vma;
extern char _text_len, _rodata_len, _bss_len, _data_len;

static inline int vmem_map_kernel(mmu_ctx_t ctx) {
	int err = 0;

	/* Map sections. */
	err |= vmem_map_on_itself(ctx, &_text_vma, (size_t) &_text_len,
			VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_rodata_vma, (size_t) &_rodata_len,
			VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_bss_vma, (size_t) &_bss_len_with_reserve,
			VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_data_vma, (size_t) &_data_len,
			VMEM_PAGE_WRITABLE);

	/* Map special info. */
	err |= vmem_map_on_itself(ctx, VIRTUAL_TABLES_START, VIRTUAL_TABLES_LEN,
			VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, VIRTUAL_PAGES_INFO_START, VIRTUAL_PAGES_INFO_LEN,
			VMEM_PAGE_WRITABLE);

	// XXX below
	// for sparc
	err |= vmem_map_on_itself(ctx, (void *) 0x80000000, (size_t) 0x1000,
			VMEM_PAGE_WRITABLE);

	// for microblaze
	err |= vmem_map_on_itself(ctx, (void *) 0x84000000, (size_t) 0x1000,
			VMEM_PAGE_WRITABLE);

	// mapping x86 video buffer
	err |= vmem_map_on_itself(ctx, (void *) 0xB8000, (size_t) 0x1000,
			VMEM_PAGE_WRITABLE);

	return err;
}

int vmem_create_context(mmu_ctx_t *ctx) {
	mmu_pgd_t *pgd = vmem_alloc_pgd_table();

	if (!pgd) {
		return -ENOMEM;
	}

	*ctx = mmu_create_context(pgd);

	return ENOERR;
}

/* FIXME: remove create context from here */
int vmem_init_context(mmu_ctx_t *ctx) {
	int err;

	if ((err = vmem_create_context(ctx))) {
		return err;
	}

	return vmem_map_kernel(*ctx);
}

void vmem_free_context(mmu_ctx_t ctx) {
	/*
	 * To unmap this context we should switch to kernel task virtual context.
	 * Actually, we can save this context for the later created tasks.
	 */

	sched_lock();
	{
		//XXX: Bad code
		mmu_set_context(1);

		vmem_unmap_region(ctx, 0, 0x80000000UL, 0);
	}
	sched_unlock();
}
