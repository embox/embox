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
#include <kernel/task.h>

#include <stdint.h>
#include <kernel/printk.h>
#include <kernel/panic.h>

/* Section pointers. */
extern char _text_vma, _rodata_vma, _data_vma, _bss_vma;
extern char _text_len, _rodata_len, _data_len, _bss_len_with_reserve;
/* phymem allocator space */
extern char *phymem_alloc_start, *phymem_alloc_end;

int vmem_map_kernel(mmu_ctx_t ctx) {
	int err = 0;
	size_t phymem_len;

	/* Map sections. */
	err |= vmem_map_on_itself(ctx, &_text_vma, (size_t) &_text_len,
			VMEM_PAGE_WRITABLE | VMEM_PAGE_EXECUTABLE);
	err |= vmem_map_on_itself(ctx, &_rodata_vma, (size_t) &_rodata_len,
			VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_data_vma, (size_t) &_data_len,
			VMEM_PAGE_WRITABLE);
	err |= vmem_map_on_itself(ctx, &_bss_vma, (size_t) &_bss_len_with_reserve,
			VMEM_PAGE_WRITABLE);


	// XXX below
	// for sparc
	err |= vmem_map_on_itself(ctx, (void *) 0x80000000, (size_t) 0x100000,
			VMEM_PAGE_WRITABLE);

	// for sparc greth
	err |= vmem_map_on_itself(ctx, (void *) 0x90000000, (size_t) 0x1000,
			VMEM_PAGE_WRITABLE);

	// for microblaze
	err |= vmem_map_on_itself(ctx, (void *) 0x84000000, (size_t) 0x1000,
			VMEM_PAGE_WRITABLE);


	// map phymem
	phymem_len = phymem_alloc_end - phymem_alloc_start;
	err |= vmem_map_on_itself(ctx, phymem_alloc_start, phymem_len, VMEM_PAGE_WRITABLE);

	return err;
}

void vmem_handle_page_fault(mmu_vaddr_t virt_addr) {
	panic("MMU page fault: virt_addr - 0x%x\n", virt_addr);
}


