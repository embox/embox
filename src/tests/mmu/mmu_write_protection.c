/**
 * @file
 *
 * @date 25.05.2010
 * @Author Anton Bondarev
 */

#include <types.h>
#include <embox/test.h>
#include <hal/mm/mmu_core.h>
#include <hal/mm/mmu_page.h>
#include <hal/env/traps_core.h>
#include <hal/test/mmu_core.h>
#include <hal/test/traps_core.h>

EMBOX_TEST(run);

static uint32_t addr;

/* MMU data access exception handler */
static int dfault_handler(uint32_t trap_nr, void *data) {

	/* skip instruction */
	return 0;
}

static int run(void) {
	extern char _text_start, __stack, _data_start;
	mmu_env_t prev_mmu_env;
	traps_env_t old_env;


	uint32_t vaddr = VADDR(&addr);

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());
	traps_save_env(&old_env);
	traps_set_env(testtraps_env());

	/* map one to one section text and may be whole image with stack */
	mmu_map_region((mmu_ctx_t)0, (uint32_t) &_text_start,
			(uint32_t) &_text_start, 0x1000000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	if (&__stack > (&_text_start + 0x1000000)) {
		/* if have to map data sections */
		mmu_map_region((mmu_ctx_t)0, (paddr_t)&_data_start, (vaddr_t)&_data_start,
				0x1000000, MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
	}

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DFAULT, dfault_handler);

	mmu_map_region((mmu_ctx_t)0, (paddr_t)&_data_start, 0xf0000000, 0x1000000,
			MMU_PAGE_CACHEABLE);

	mmu_on();

	*((volatile uint32_t *)vaddr) = 0x11111111;

	traps_restore_env(&old_env);
	mmu_restore_env(&prev_mmu_env);

	return (addr == 0x11111111) ? -1 : 0;
}
