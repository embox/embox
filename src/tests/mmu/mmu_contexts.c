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
#include <hal/test/mmu_core.h>
#include <string.h>

EMBOX_TEST(run);

//static char addr[0x1000 * 3];
typedef void (*worker_ptr)(void);

static int is_a_done = 0;
static int is_b_done = 0;

#define BIGADDR 0xf0080000

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

static int run(void) {
	extern char _text_start, __stack, _data_start;
	extern char worker_a_aligned, worker_b_aligned;
	worker_ptr temp;

	int status = 0;
	int i;
	mmu_env_t prev_mmu_env;
	mmu_env_t *cur_mmu_env;
	mmu_ctx_t t[3];
	mmu_save_env(&prev_mmu_env);
	cur_mmu_env = testmmu_env();
	mmu_set_env(cur_mmu_env);

	/* have to do this, otherwise compiler throw out
	 * worker_a and worker_b */
	temp = &worker_a;
	temp = &worker_b;
	t[0] = 0;
	t[1] = mmu_create_context();
	t[2] = mmu_create_context();
	for (i = 0; i <= 2; i++) {
		mmu_map_region(t[i], (uint32_t) &_text_start,
				(uint32_t) &_text_start, 0x1000000,
				MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
		if (&__stack > (&_text_start + 0x1000000)) {
			/* if have to map data sections */
			mmu_map_region(t[i], _data_start, _data_start, 0x1000000,
					MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);
		}
		mmu_map_region((mmu_ctx_t) t[i], (uint32_t) 0x80000000,
				(uint32_t) 0x80000000, 0x1000000, MMU_PAGE_WRITEABLE);
	}
	mmu_map_region(t[1], (paddr_t) &worker_a_aligned, BIGADDR, 0x1000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
	mmu_map_region(t[2], (paddr_t) &worker_b_aligned, BIGADDR, 0x1000,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);
	i = * ((int *) &worker_a_aligned);
	switch_mm(0, t[0]);
	mmu_on();
	printf("\n\n %8x %8x\n",i, *((int *) BIGADDR));
	(*((worker_ptr) BIGADDR))();
	mmu_off();
	switch_mm(t[1], t[2]);
	mmu_on();
	(*((worker_ptr) BIGADDR))();
	mmu_off();
	switch_mm(t[2], 0);
	//printf("a is %d \nb is %d\n",is_a_done, is_b_done);
#if 1
	if (is_a_done)
		printf("call a was done\n");
	if (is_b_done)
		printf("call b was done\n");

#endif
	mmu_restore_env(&prev_mmu_env);
	status = !(is_a_done && is_b_done);
	return status;
}
