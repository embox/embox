/**
 * @file
 *
 * @brief Testing page fault in virtual mode
 *
 * @date 07.04.2010
 * @author Nikolay Korotky
 */

#include <types.h>

#include <embox/test.h>
#include <hal/mm/mmu_core.h>
#include <hal/env/traps_core.h>
#include <hal/test/testmmu_core.h>
#include <hal/test/testtraps_core.h>

/* declare test in system */
EMBOX_TEST(run);

#define MMU_DFAULT 0x09

static unsigned int volatile test_var;

/* MMU data access exception handler */
static void dfault_handler(uint32_t trap_nr, void *data) {
	test_var++;
}

static int run() {
	extern char _text_start, __stack, _data_start;
	unsigned int tmp = test_var;
	mmu_env_t prev_mmu_env;
	traps_env_t old_env;
	int status;

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());
	traps_save_env(&old_env);
	traps_set_env(testtraps_env());

	/* map one to one section text and may be whole image with stack */
	mmu_map_region((mmu_ctx_t)0, (uint32_t) &_text_start, (uint32_t) &_text_start, 0x1000000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	if (&__stack > (&_text_start + 0x1000000)) {
		/* if have to map data sections */
		mmu_map_region((mmu_ctx_t)0, _data_start, _data_start, 0x1000000, MMU_PAGE_CACHEABLE
				| MMU_PAGE_WRITEABLE);
	}
	mmu_map_region((mmu_ctx_t)0, 0x40000000, 0xf0080000, 0x40000, MMU_PTE_PRIV_RDONLY);

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DFAULT, dfault_handler);

	mmu_on();

	/* Data access exception */
	*((volatile unsigned long *)0xf0091004) = 0x66633666;

	traps_restore_env(&old_env);
	mmu_restore_env(&prev_mmu_env);

	return (tmp != (test_var - 1)) ? -1 : 0;
}
