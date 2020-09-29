/**
 * @file
 * @brief
 *
 * @date 21.06.2013
 * @author Anton Bondarev
 */

#include <framework/mod/options.h>

#if OPTION_GET(NUMBER, task_quantity)

#include <kernel/task/resource.h>

#include <hal/mmu.h>

#include <kernel/thread/thread_alloc.h>
#include <mem/page.h>

#include <kernel/thread.h>
#include <mem/misc/pool.h>
#include <assert.h>

#include <kernel/thread/stack_protect.h>
#include <util/binalign.h>

#define THREAD_STACK_SZ \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_stack_size)
static_assert(THREAD_STACK_SZ > sizeof(struct thread));

#define POOL_SZ       OPTION_GET(NUMBER, task_quantity)

#define RESOURCE_SIZE OPTION_GET(NUMBER, resource_size)

#define STACK_SZ      \
	binalign_bound(THREAD_STACK_SZ + RESOURCE_SIZE, THREAD_STACK_ALIGN)

typedef union thread_pool_entry {
	struct thread thread;
	char stack[STACK_SZ];
} thread_pool_entry_t;

#ifdef STACK_PROTECT_MMU
#include <mem/vmem.h>
POOL_DEF_ATTR(thread_pool, thread_pool_entry_t, POOL_SZ,
    __attribute__ ((aligned (VMEM_PAGE_SIZE))));
#else
POOL_DEF(main_thread_pool, thread_pool_entry_t, POOL_SZ);
#endif

struct thread *main_thread_alloc(void) {
	thread_pool_entry_t *block;
	struct thread *t;

	if (!(block = (thread_pool_entry_t *) pool_alloc(&main_thread_pool))) {
		return NULL;
	}
	memset(block, 0x53, sizeof(*block));

	t = &block->thread;

	thread_stack_init(t, STACK_SZ);

    stack_protect(t, STACK_SZ);

	return t;
}

void main_thread_free(struct thread *t) {
	thread_pool_entry_t *block;

	assert(t != NULL);

	// TODO may be this is not the best way... -- Eldar
	block = member_cast_out(t, thread_pool_entry_t, thread);

    stack_protect_release(t);

	memset(block, 0xa5, sizeof(*block));

	pool_free(&main_thread_pool, block);
}

#endif /* OPTION_GET(NUMBER, task_quantity) */
