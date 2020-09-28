/**
 * @file
 * @brief
 *
 * @date 21.06.2013
 * @author Anton Bondarev
 */

#include <hal/mmu.h>

#include <kernel/thread/thread_alloc.h>
#include <mem/page.h>

#include <kernel/thread.h>
#include <mem/misc/pool.h>
#include <assert.h>

#include <kernel/thread/stack_protect.h>

#define STACK_SZ     THREAD_DEFAULT_STACK_SIZE
static_assert(STACK_SZ > sizeof(struct thread));

#define STACK_ALIGN \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, stack_align)

#define POOL_SZ \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_pool_size)

typedef union thread_pool_entry {
	struct thread thread;
	char stack[STACK_SZ];
} thread_pool_entry_t;

#ifdef STACK_PROTECT_MMU
#include <mem/vmem.h>
POOL_DEF_ATTR(thread_pool, thread_pool_entry_t, POOL_SZ,
    __attribute__ ((aligned (VMEM_PAGE_SIZE))));
#else
POOL_DEF_ATTR(thread_pool, thread_pool_entry_t, POOL_SZ,
	__attribute__ ((aligned (STACK_ALIGN))));
#endif

struct thread *thread_alloc(size_t stack_sz) {
	thread_pool_entry_t *block;
	struct thread *t;

	(void) stack_sz;

	if (!(block = (thread_pool_entry_t *) pool_alloc(&thread_pool))) {
		return NULL;
	}
	memset(block, 0x53, sizeof(*block));

	t = &block->thread;

	thread_stack_init(t, STACK_SZ);

    stack_protect(t, STACK_SZ);

	return t;
}

void thread_free(struct thread *t) {
	thread_pool_entry_t *block;

	assert(t != NULL);

	// TODO may be this is not the best way... -- Eldar
	block = member_cast_out(t, thread_pool_entry_t, thread);

    stack_protect_release(t);

	memset(block, 0xa5, sizeof(*block));

	pool_free(&thread_pool, block);
}
