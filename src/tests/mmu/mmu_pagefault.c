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
#include <asm/tbr.h>

EMBOX_TEST(run);

uint8_t page[PAGE_SIZE * 2];
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
	extern char _text_start, __stack, _data_start;
	extern int test_trap_handler_worked;
	extern trap_table;
	mmu_env_t prev_mmu_env;
	traps_env_t old_env, *test_env;
	int t = 0;

	printf("tba %x; main_trab_table %x\n",tbr_tba_get(), &trap_table);
	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());
	traps_save_env(&old_env);
	test_env = testtraps_env();
	traps_set_env(test_env);

	printf("handler on %x; tbr %x; in env %x, trap_table %x\n",
		&handler, tbr_tba_get(), test_env->base_addr, &__test_trap_table);
	t1 = mmu_create_context();
	switch_mm(0,t1);
	//testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x2a , &handler); // div by zero
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x2c - CONFIG_MIN_HWTRAP_NUMBER, &handler); //data mmu miss
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x29 - CONFIG_MIN_HWTRAP_NUMBER, &handler); //data mmu error
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x9, &handler);  //data except
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x3c - CONFIG_MIN_HWTRAP_NUMBER, &handler); //instr mmu miss
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x1, &handler);  //instr excpt
	printf("text_start %x; stack %x, data_start %x\n", &_text_start, &__stack, &_data_start);
/* instr mmu miss */
#if 1
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
/* div by zero */
#if 0
	//now printing about no existance of handler -- OK
	t = 1 / 0;
	t = 2 / 0;
#endif

/* data mmu miss */
#if 1
	//segfault -- bad
	mmu_on();

	//t = 1 / 0;
	*((unsigned long *) 0xf0080000) = 0;

	mmu_off();
#endif
	switch_mm(t1,0);
	mmu_delete_context(t1);
	printf("flag is %d\n",flag);
	//traps_restore_env(&old_env);
	mmu_restore_env(&prev_mmu_env);

	return result;
}
