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
//static char addr[0x1000 * 3];
typedef void (*worker_ptr)(void);

static int is_a_done = 0;
static int is_b_done = 0;

static void worker_a(void) {
	__asm__  __volatile__ (
			".align 0x1000\n\t"
			"worker_a_aligned:\n\t"
	);
	is_a_done = 1;
}

static void worker_b(void) {
	__asm__   __volatile__(
			".align 0x1000\n\t"
			"worker_b_aligned:\n\t"
	);
	is_b_done = 1;
}

static int run() {
	extern char _text_start, __stack, _data_start;
	extern char worker_a_aligned, worker_b_aligned;
	worker_ptr temp;

	int status = 0;
	int i;
	mmu_env_t prev_mmu_env;
	mmu_env_t *cur_mmu_env;
	mmu_ctx_t t1, t2;
	mmu_save_env(&prev_mmu_env);
	cur_mmu_env = testmmu_env();
	mmu_set_env(cur_mmu_env);

	/* have to do this, otherwise compiler throw out
	 * worker_a and worker_b */
	temp = &worker_a;
	temp = &worker_b;

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
	mmu_map_region(t1, (paddr_t) &worker_a_aligned, 0xf0080000, 0x1000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
	mmu_map_region(t2, (paddr_t) &worker_b_aligned, 0xf0080000, 0x1000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	mmu_on();
	switch_mm(0, t1);
	(*((worker_ptr) 0xf0080000))();
	mmu_off();
	switch_mm(t1, t2);
	mmu_on();
	(*((worker_ptr) 0xf0080000))();
	mmu_off();
	switch_mm(t2, 0);
	//printf("a is %d b is %d\n",is_a_done, is_b_done);
	status = !(is_a_done && is_b_done);
	return status;
}
