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
#include <hal/mmu.h>
#include <assert.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(thread_allocator_heap_init);

#define THREAD_HEAP_SIZE     OPTION_GET(NUMBER, thread_heap_size)

static struct page_allocator *thread_heap_allocator;

struct thread *thread_alloc(size_t stack_sz) {
	struct thread *t;
	size_t pages = (stack_sz + PAGE_SIZE() - 1) / PAGE_SIZE();

	t = (struct thread *) page_alloc(thread_heap_allocator, pages);
	if (!t) {
		return NULL;
	}
	memset(t, 0x53, stack_sz);
	*(uint32_t *) (t + 1) = pages;

	thread_stack_set(t, (uint8_t *)(t + 1) + 4);
	thread_stack_set_size(t, stack_sz - sizeof *t - 4);

	stack_protect(t, stack_sz);

	return t;
}

void thread_free(struct thread *t) {
	size_t pages;

	assert(t != NULL);

	stack_protect_release(t);

	pages = *(uint32_t *) (t + 1);

	page_free(thread_heap_allocator, t, pages);

	memset(t, 0xa5, pages * PAGE_SIZE());
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
