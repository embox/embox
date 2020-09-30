/**
 * @file
 * @brief
 *
 * @date 21.06.2013
 * @author Anton Bondarev
 */

#include <assert.h>

#include <hal/mmu.h>

#include <kernel/thread/thread_alloc.h>
#include <kernel/thread.h>
#include <kernel/thread/stack_protect.h>

#include <mem/page.h>
#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#define USE_USER_STACK    OPTION_GET(NUMBER, thread_user_stack)

#define STACK_SZ          THREAD_DEFAULT_STACK_SIZE

#define POOL_SZ \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_pool_size)

#if (USE_USER_STACK == 0)

typedef union thread_pool_entry {
	struct thread thread;
	char stack[STACK_SZ];
} thread_pool_entry_t;

#else

extern void *thread_user_stack_alloc(struct thread *t, size_t stack_sz);
extern void thread_user_stack_free(void *t);

typedef struct thread_pool_entry {
	struct thread thread;
	void *user_stack;
} thread_pool_entry_t;

#endif

#ifdef STACK_PROTECT_MMU
#include <mem/vmem.h>
POOL_DEF_ATTR(thread_pool, thread_pool_entry_t, POOL_SZ,
		__attribute__ ((aligned (VMEM_PAGE_SIZE))));
#else
POOL_DEF_ATTR(thread_pool, thread_pool_entry_t, POOL_SZ,
		__attribute__ ((aligned (THREAD_STACK_ALIGN))));
#endif

struct thread *thread_alloc(size_t stack_sz) {
	thread_pool_entry_t *block;
	struct thread *t;

	if (!(block = pool_alloc(&thread_pool))) {
		return NULL;
	}
	memset(block, 0x53, sizeof(*block));

	t = &block->thread;
#if (USE_USER_STACK == 0)
	stack_sz = STACK_SZ;
	thread_stack_init(t, stack_sz);
#else
	block->user_stack = thread_user_stack_alloc(t, stack_sz);
#endif

	stack_protect(t, stack_sz);

	return t;
}

void thread_free(struct thread *t) {
	thread_pool_entry_t *block;

	assert(t != NULL);

	block = (thread_pool_entry_t *)t;

	stack_protect_release(t);

#if (USE_USER_STACK == 0)

#else
	thread_user_stack_free(block->user_stack);
#endif

	memset(block, 0xa5, sizeof(*block));

	pool_free(&thread_pool, block);
}
