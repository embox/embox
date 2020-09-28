/**
 * @file
 * @brief
 *
 * @date   18.09.2020
 * @author Alexander Kalmuk
 */

#include <kernel/thread.h>
#include <kernel/thread/thread_alloc.h>
#include <kernel/thread/stack_protect.h>
#include <mem/page.h>
#include <mem/misc/pool.h>
#include <hal/mmu.h>
#include <assert.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(thread_allocator_heap_init);

#define THREAD_HEAP_SIZE     OPTION_GET(NUMBER, thread_heap_size)

#define POOL_SZ \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_pool_size)

struct thread_info {
	struct thread thread;
	size_t pages;
	void *stack;
};

POOL_DEF(thread_info_pool, struct thread_info, POOL_SZ);

static struct page_allocator *thread_heap_allocator;

struct thread *thread_alloc(size_t stack_sz) {
	struct thread_info *ti;
	uint8_t *stack;
	size_t pages = (stack_sz + PAGE_SIZE() - 1) / PAGE_SIZE();

	stack_sz = pages * PAGE_SIZE();

	ti = (struct thread_info *) pool_alloc(&thread_info_pool);
	if (!ti) {
		return NULL;
	}

	stack = (uint8_t *) page_alloc(thread_heap_allocator, pages);
	if (!stack) {
		pool_free(&thread_info_pool, ti);
		return NULL;
	}
	memset(stack, 0x53, stack_sz);

	ti->stack = stack;
	ti->pages = pages;

	thread_stack_set(&ti->thread, stack);
	thread_stack_set_size(&ti->thread, stack_sz);

	stack_protect(&ti->thread, stack_sz);

	return &ti->thread;
}

void thread_free(struct thread *t) {
	struct thread_info *ti = (struct thread_info *) t;

	assert(t != NULL);

	stack_protect_release(t);

	page_free(thread_heap_allocator, ti->stack, ti->pages);
	memset(ti->stack, 0xa5, ti->pages * PAGE_SIZE());

	pool_free(&thread_info_pool, ti);
}

static int thread_allocator_heap_init(void) {
	extern char thread_heap_start;

	thread_heap_allocator = page_allocator_init((char *)&thread_heap_start,
		THREAD_HEAP_SIZE, PAGE_SIZE());
	if (!thread_heap_allocator) {
		return -1;
	}

	return 0;
}
