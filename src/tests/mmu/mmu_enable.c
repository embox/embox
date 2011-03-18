/**
 * @file
 * @brief Testing page translating in virtual mode
 *
 * @details This is the simplest test for MMU module. It just maps one to one
 *         memory region where lay sections '.text', '.data', '.bss' and stack.
 *         And than switch on virtual mode, and immediately restore mode
 *         (virtual or real).
 *
 * @date 09.03.10
 * @author Anton Bondarev
 */

#include <types.h>

#include <embox/test.h>
#include <hal/mm/mmu_core.h>
#include <hal/test/mmu_core.h>

EMBOX_TEST(run);

/* starting function for test */
static int run(void) {
	extern char _text_start, __stack, _data_start;
	mmu_env_t prev_mmu_env;

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());

	printf("\n\nGTABLE MASK %8lx \nMTABLE MASK %8lx\nPTABLE_MASK %8lx\n\n",
			MMU_GTABLE_MASK, MMU_MTABLE_MASK, MMU_PTABLE_MASK);
	/* map one to one section text and may be whole image with stack */
	mmu_map_region((mmu_ctx_t) 0, (uint32_t) &_text_start,
		(uint32_t) &_text_start, 0x1000000, MMU_PAGE_CACHEABLE
		| MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	if (&__stack > (&_text_start + 0x10000000)) {
		/* if have to map data sections */
		mmu_map_region((mmu_ctx_t) 0, (paddr_t) &_data_start,
			(vaddr_t) &_data_start, 0x1000000,
			(uint32_t)(MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE));
	}

	mmu_on();
	mmu_restore_env(&prev_mmu_env);

	return 0;
}
