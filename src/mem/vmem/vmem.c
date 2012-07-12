/**
 * @file
 * @brief Virtual memory subsystem
 *
 * @date 29.07.11
 * @author Anton Kozlov
 */

#include <embox/unit.h>

#include <hal/mm/mmu_core.h>
#include <hal/env/traps_core.h>
#include <hal/test/traps_core.h>
#include <mem/pagealloc/opallocator.h>
#include <mem/page.h>

EMBOX_UNIT_INIT(vmem_init);


void vmem_on(void) {
	mmu_on();
}

void vmem_off(void) {
	mmu_off();
}

#define REGION_SIZE 0x1000000

static int pagefault_handler(uint32_t nr, void *data) {
	int err_addr = mmu_get_fault_address() & ~(PAGE_SIZE() - 1);
	mmu_off();

	mmu_map_region((mmu_ctx_t) 0, (paddr_t) page_alloc() ,
			(vaddr_t) err_addr & ~(PAGE_SIZE() - 1), PAGE_SIZE(), MMU_PAGE_CACHEABLE |
			MMU_PAGE_WRITEABLE);

	mmu_on();
	return 1; // execute exception-cause instruction once more
}


static int vmem_init(void) {
	//extern char _text_start;
	//extern char _heap_start;

	mmu_map_region((mmu_ctx_t)0, (paddr_t) 0x44000000,
			(vaddr_t) 0x44000000, REGION_SIZE,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	mmu_map_region((mmu_ctx_t)0, (paddr_t) 0x84000000,
				(vaddr_t) 0x84000000, REGION_SIZE,
				MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_MISS_FAULT,
					&pagefault_handler); //data mmu miss


	vmem_on();

	return 0;
}
