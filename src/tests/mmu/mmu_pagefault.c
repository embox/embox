/**
 * @file
 * @brief Testing automatic mapping on unmapped virtual address space
 *
 * @author Anton Kozlov
 *
 * @date 04.05.10
 */

#include <embox/test.h>
#include <hal/mm/mmu_core.h>
#include <hal/env/traps_core.h>
#include <hal/test/mmu_core.h>
#include <hal/test/traps_core.h>

EMBOX_TEST(run);

#define PAGE_SIZE 0x1000

#define MAGIC_CONST 0x12345678

#define FAULT_ADDR ((unsigned long *) 0xf0000000)

uint8_t __attribute__ ((aligned (PAGE_SIZE))) page[PAGE_SIZE];

mmu_ctx_t t0 = (mmu_ctx_t) 0;

static int flag = 0;

static int pagefault_handler(uint32_t nr, void *data) {
	int err_addr = mmu_get_fault_address() & ~(PAGE_SIZE - 1);
	mmu_off();

	mmu_map_region(t0, (unsigned long *) ((unsigned long) &page),
			err_addr, PAGE_SIZE, MMU_PAGE_CACHEABLE |
			MMU_PAGE_WRITEABLE);

	flag = 1;
	mmu_on();
	return 1; // execute exception-cause instruction once more
}
static int run(void) {
	int magic_val = 0;
	extern char _text_start, __stack, _data_start;

	mmu_env_t prev_mmu_env;
	traps_env_t old_env, *test_env;

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());

	traps_save_env(&old_env);
	test_env = testtraps_env();
	traps_set_env(test_env);

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_MISS_FAULT,
			&pagefault_handler); //data mmu miss

	mmu_map_region(t0, (uint32_t) &_text_start,
				(uint32_t) &_text_start, 0x1000000,
				MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
	/* instr mmu miss */

	/* map all that we seems to have to map (_everything_)*/
#if 0
	mmu_map_region(t1, 0, 0, 0x40000000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	/* map one to one section text and may be whole image with stack */


	if (&__stack > (&_text_start + 0x1000000)) {
		/* if have to map data sections */
		mmu_map_region(t1, _data_start, _data_start, 0x1000000,
				MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
	}
#endif

/* data mmu miss */
	mmu_on();
	*(FAULT_ADDR) = MAGIC_CONST;
	magic_val = *((unsigned long *) FAULT_ADDR);
	mmu_off();


	traps_restore_env(&old_env);
	mmu_restore_env(&prev_mmu_env);

	return magic_val == MAGIC_CONST ? 0 : -1;
}

