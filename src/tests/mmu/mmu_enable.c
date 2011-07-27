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
#include <hal/ipl.h>
#include <asm/cache.h>

EMBOX_TEST(run);

#define MMAP_SIZE 0x1000000

/* starting function for test */
static int run(void) {
	extern char _text_start, _stack_top, _data_image_start;

	mmu_env_t prev_mmu_env;

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());

	/* map one to one section text and may be whole image with stack */
	mmu_map_region((mmu_ctx_t) 0, (uint32_t) &_text_start,
		(uint32_t) &_text_start, MMAP_SIZE, MMU_PAGE_CACHEABLE
		| MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
#if 0
	mmu_map_region((mmu_ctx_t) 0, (uint32_t) 0x84000000,
			(uint32_t) 0x84000000, MMAP_SIZE, MMU_PAGE_CACHEABLE
			| MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
#endif
	if (&_stack_top > (&_text_start + MMAP_SIZE)) {
		/* if have to map data sections */
		/*XXX cheking for stack, mapping data */
		printf("mapping stack\n");

		mmu_map_region((mmu_ctx_t) 0, (paddr_t) ((int) &_stack_top - MMAP_SIZE),
			(vaddr_t) ((int) &_stack_top - MMAP_SIZE), MMAP_SIZE,
			(uint32_t)(MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE));
	}
#if 0
	mmu_map_region((mmu_ctx_t) 0, (paddr_t) &_data_image_start,
				(vaddr_t) &_data_image_start, MMAP_SIZE,
				(uint32_t)(MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE));
#endif

	mmu_on();
	mmu_restore_env(&prev_mmu_env);

	return 0;
}
