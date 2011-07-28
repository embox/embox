/**
 * @file
 *
 * @date 25.05.10
 * @author Anton Bondarev
 */

#include <types.h>

#include <embox/test.h>
#include <hal/mm/mmu_core.h>
#include <hal/mm/mmu_page.h>
#include <hal/env/traps_core.h>
#include <hal/test/mmu_core.h>
#include <hal/test/traps_core.h>

EMBOX_TEST(mmu_mark_page_run);

static uint32_t addr;

#define MMAP_SIZE 0x1000000

#define BIGADDR 0xf0000000

#define VADDR(region, phyaddr) (region + ((uint32_t)(phyaddr) & (MMAP_SIZE - 1)))

#define MAGIC_CONST 0x12345678

/* MMU data access exception handler */
static int dfault_handler(uint32_t trap_nr, void *data) {
	mmu_page_set_flags((mmu_ctx_t) 0x0, BIGADDR, MMU_PAGE_WRITEABLE);
	/* repeat instruction */
	return 1;
}

static int mmu_mark_page_run(void) {
	extern char _text_start, __stack, _data_image_start;
	mmu_env_t prev_mmu_env;
	traps_env_t old_env;

	uint32_t vaddr = VADDR(BIGADDR, &addr);

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());

	traps_save_env(&old_env);
	traps_set_env(testtraps_env());

	/* map one to one section text and may be whole image with stack */
	mmu_map_region((mmu_ctx_t)0, (uint32_t) &_text_start,
			(uint32_t) &_text_start, 0x1000000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
#if 0
	if (&__stack > (&_text_start + 0x1000000)) {
		/* if have to map data sections */
		mmu_map_region((mmu_ctx_t)0, (paddr_t)&_data_image_start, (vaddr_t)&_data_image_start,
				0x1000000, MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
	}
#endif

//TODO mmu fix direct io map
/*	mmu_map_region((mmu_ctx_t) 0x0, (uint32_t) 0x80000000,
			(uint32_t) 0x80000000, 0x1000000, MMU_PAGE_WRITEABLE);
*/
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_SECUR_FAULT, dfault_handler);

	mmu_map_region((mmu_ctx_t)0, (paddr_t)&_data_image_start & ~(MMAP_SIZE - 1), BIGADDR, MMAP_SIZE,
			MMU_PAGE_CACHEABLE);

	mmu_on();

	*((volatile uint32_t *) vaddr) = MAGIC_CONST;


	traps_restore_env(&old_env);
	mmu_restore_env(&prev_mmu_env);

	return (addr != MAGIC_CONST) ? -1 : 0;
}
