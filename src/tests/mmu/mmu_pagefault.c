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

//uint8_t page[PAGE_SIZE * 2];
mmu_ctx_t t1;
static int flag = 0;

int handler(uint32_t nr, void *data) {
	//mmu_off();
	//printf("IT'S A TRAP!\n");
	//mmu_map_region(t1, (unsigned long *) ((unsigned long) &page + PAGE_SIZE), 0xf0080000, PAGE_SIZE,
					//MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
	flag++;
	//mmu_on();
	return 0;
}
static int run(void) {
	int result = 0;
#if 0
	extern char _text_start, __stack, _data_start;
	extern int test_trap_handler_worked;
//	extern trap_table;
#endif
	mmu_env_t prev_mmu_env;
	traps_env_t old_env, *test_env;

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());

	traps_save_env(&old_env);
	test_env = testtraps_env();
	traps_set_env(test_env);

	t1 = mmu_create_context();
	switch_mm(0,t1);
#if 0
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x2c - CONFIG_MIN_HWTRAP_NUMBER, &handler); //data mmu miss
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x29 - CONFIG_MIN_HWTRAP_NUMBER, &handler); //data mmu error
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x9, &handler);  //data except
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x3c - CONFIG_MIN_HWTRAP_NUMBER, &handler); //instr mmu miss
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x1, &handler);  //instr excpt

	/* instr mmu miss */

	/* map all that we seems to have to map (_everything_)*/
	mmu_map_region(t1, 0, 0, 0x40000000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
	/* map one to one section text and may be whole image with stack */
	mmu_map_region(t1, (uint32_t) &_text_start,
			(uint32_t) &_text_start, 0x1000000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	if (&__stack > (&_text_start + 0x1000000)) {
		/* if have to map data sections */
		mmu_map_region(t1, _data_start, _data_start, 0x1000000,
				MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
	}
#endif

/* data mmu miss */
#if 0
	mmu_on();
	*((unsigned long *) 0xf0080000) = 0;
	mmu_off();
#endif

	switch_mm(t1,0);
	mmu_delete_context(t1);

	traps_restore_env(&old_env);
	mmu_restore_env(&prev_mmu_env);

	return result;
}

