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

EMBOX_UNIT_INIT(vmem_init);

#define PG_SIZE CONFIG_PAGE_SIZE

void vmem_on(void) {
	mmu_on();
}

void vmem_off(void) {
	mmu_off();
}

#define REGION_SIZE 0x1000000

static int pagefault_handler(uint32_t nr, void *data) {
	int err_addr = mmu_get_fault_address() & ~(PG_SIZE - 1);
	mmu_off();

	mmu_map_region((mmu_ctx_t) 0, (paddr_t) page_alloc() ,
			(vaddr_t) err_addr & ~(PG_SIZE - 1), PG_SIZE, MMU_PAGE_CACHEABLE |
			MMU_PAGE_WRITEABLE);

	mmu_on();
	return 1; // execute exception-cause instruction once more
}

#if 0
static size_t mmap_size(size_t size) {
	size_t  ret = size & ~(PG_SIZE - 1);
	if (size & (PG_SIZE - 1)) {
		ret += PG_SIZE;
	}
	return ret;
}
#endif
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


