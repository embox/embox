/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 29.07.11
 * @author Anton Kozlov
 */

#include <embox/unit.h>

#include <types.h>
#include <stdio.h>
#include <hal/mm/mmu_core.h>
#include <hal/env/traps_core.h>
#include <hal/test/traps_core.h>

extern char __KERNEL_START;
extern char __KERNEL_END;

#define KERNEL_START 	(&__KERNEL_START)
#define KERNEL_END		(&__KERNEL_END)
#define KERNEL_SIZE     (KERNEL_END - KERNEL_START)

EMBOX_UNIT(vmem_init, vmem_fini);

static void vmem_map_kernel(mmu_ctx_t ctx);

void vmem_on(void) {
	mmu_on();
}

void vmem_off(void) {
	mmu_off();
}

static void vmem_map_kernel(mmu_ctx_t ctx) {
	mmu_map_region(ctx, (paddr_t) KERNEL_START, (vaddr_t) KERNEL_START,
			KERNEL_SIZE, MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
}


/*

static int pagefault_handler(uint32_t nr, void *data) {
	int err_addr = mmu_get_fault_address() & ~(PAGE_SIZE() - 1);
	mmu_off();

	mmu_map_region((mmu_ctx_t) 0, (paddr_t) page_alloc() ,
			(vaddr_t) err_addr & ~(PAGE_SIZE() - 1), PAGE_SIZE(), MMU_PAGE_CACHEABLE |
			MMU_PAGE_WRITEABLE);

	mmu_on();
	return 1; // execute exception-cause instruction once more
}

*/
static int vmem_init(void) {
#if 0
	printf("\n\n");
	printf("KERNEL_START = 0x%08x\n", (unsigned int) KERNEL_START);
	printf("KERNEL_END = 0x%08x\n", (unsigned int) KERNEL_END);
	printf("KERNEL_SIZE = 0x%08x\n", (unsigned int) KERNEL_SIZE);
	printf("\n");
#endif

	vmem_map_kernel((mmu_ctx_t) 0);
	vmem_on();

	return 0;
}

static int vmem_fini(void) {
	vmem_off();

	return 0;
}

