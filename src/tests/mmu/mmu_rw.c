/**
 * @file
 * @brief Testing read/write in virtual mode.
 *
 * @date 05.04.2010
 * @author Nikolay Korotky
 */

#include <types.h>
#include <embox/test.h>
#include <hal/mm/mmu_core.h>
#include <hal/test/testmmu_core.h>

/* declare test in system */
EMBOX_TEST(run);
static char addr[0x1000 * 3];
//static char *pointer;

/* starting function for test */
static int run() {
	extern char _text_start, __stack, _data_start;
	int status = 0;
	int i;
	mmu_env_t prev_mmu_env;

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());

	/* map one to one section text and may be whole image with stack */
	mmu_map_region((mmu_ctx_t)0, (uint32_t) &_text_start,
			(uint32_t) &_text_start, 0x1000000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	if (&__stack > (&_text_start + 0x1000000)) {
		/* if have to map data sections */
		mmu_map_region((mmu_ctx_t)0, _data_start, _data_start, 0x1000000,
				MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
	}
	mmu_map_region((mmu_ctx_t)0, (paddr_t)&addr, 0xf0080000, 0x40000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
	mmu_on();
	if ((*((unsigned long *)0xf0080000)) != (*((unsigned long *)&addr))) {
		status = -1;
	}

	/* test read/write */
	*((volatile unsigned long *)0xf0091004) = 0x87654321;
	if ( (*((volatile unsigned long *)0xf0091004)) != 0x87654321 ) {
		status = -1;
	}

	mmu_restore_env(&prev_mmu_env);
	return status;
}
