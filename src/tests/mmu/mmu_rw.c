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
#include <hal/test/mmu_core.h>
#include <asm/hal/mm/mmu_core.h>

EMBOX_TEST(run);

static uint32_t addr;

unsigned long mmu_get_fault_reg(void) {
	return mmu_get_mmureg(LEON_CNR_F);
}

unsigned long mmu_get_fault_addr(void) {
	return mmu_get_mmureg(LEON_CNR_FADDR);
}

/* starting function for test */
static int run(void) {
	extern char _text_start, __stack, _data_start;
	int status = 0;

	mmu_env_t prev_mmu_env;
	uint32_t vaddr = VADDR(&addr);

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());

	/* map one to one section text and may be whole image with stack */
	mmu_map_region((mmu_ctx_t)0, (uint32_t) &_text_start,
			(uint32_t) &_text_start, 0x1000000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	if (&__stack > (&_text_start + 0x1000000)) {
		/* if have to map data sections */
		mmu_map_region((mmu_ctx_t)0, (paddr_t) &_data_start,
				(vaddr_t) &_data_start, 0x1000000,
				MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
	}

	mmu_map_region((mmu_ctx_t)0, (paddr_t)&_data_start, 0xf0000000, 0x1000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);

	printf("%lu\n", mmu_get_fault_reg());
	mmu_on();

	if ((*((volatile uint32_t *) vaddr)) != (*((unsigned long *) &addr))) {
		status = -1;
	}

	/* test read/write */
	*((volatile uint32_t *) vaddr) = 0x87654321;
	if (*((volatile uint32_t *) vaddr) != 0x87654321) {
		status = -2;
	}

	mmu_restore_env(&prev_mmu_env);

	return status;
}
