/**
 * @file
 * @brief Testing mapping with different contexts
 *
 * @date 17.04.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <hal/mm/mmu_core.h>
#include <hal/test/testmmu_core.h>
#include <string.h>
/* declare test in system */
EMBOX_TEST(run);
static char addr[0x1000 * 3];
typedef void (*worker_ptr)();
worker_ptr *pointer;

static int is_a_done = 0;
static int is_b_done = 0;
static void worker_a() {
	__asm__  __volatile__ (
			".align 0x1000\n\t"
			"worker_a_aligned:"
			//"sethi 0, %%g1\n\t"
			//"st %%g1, [%0]\n\t"
		: "=r"(is_a_done)
		:
		: "g1"
	);
}

static void worker_b() {
	__asm__   __volatile__(
			".align 0x1000\n\t"
			"worker_b_aligned:"
			//"sethi 0, %%g1\n\t"
			//"st %%g1, [%0]\n\t"
		: "=r"(is_b_done)
		:
		: "g1"
	);
}


static int run() {
	extern char _text_start, __stack, _data_start;
	extern unsigned long worker_a_aligned, worker_b_aligned;
	int status = 0;
	int i;
	mmu_env_t prev_mmu_env;
	mmu_env_t *cur_mmu_env;
	mmu_ctx_t t1, t2;
	mmu_save_env(&prev_mmu_env);
	cur_mmu_env = testmmu_env();
	mmu_set_env(cur_mmu_env);
	printf("%x %x\n", worker_a_aligned, worker_b_aligned);
	pointer = (worker_ptr *) (((unsigned long)addr + 0x1000) & ~0xfff);
	*pointer = &worker_a;
	*((worker_ptr *) (((unsigned long) pointer) + 0x1000 )) = &worker_b;

	t1 = mmu_create_context();
    t2 = mmu_create_context();
	for (i = 0; i <= 2; i++) {
		mmu_map_region((mmu_ctx_t) i, (uint32_t) &_text_start,
				(uint32_t) &_text_start, 0x1000000,
				MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
		if (&__stack > (&_text_start + 0x1000000)) {
			/* if have to map data sections */
			mmu_map_region((mmu_ctx_t) i, _data_start, _data_start, 0x1000000,
					MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
		}
	}
	mmu_map_region(t1, (paddr_t) pointer, 0xf0080000, 0x1000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
	mmu_map_region(t2, (paddr_t) (((unsigned long) pointer) + 0x1000), 0xf0080000, 0x1000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	mmu_on();
	switch_mm(0, t1);
	(*((worker_ptr *) 0xf0080000))();
	mmu_off();
	switch_mm(t1, t2);
	mmu_on();
	(*((worker_ptr *) 0xf0080000))();
	mmu_off();
	printf("a is %d b is %d\n",is_a_done, is_b_done);
	status = !(is_a_done && is_b_done);
	return status;
}
