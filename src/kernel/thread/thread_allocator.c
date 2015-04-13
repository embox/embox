/**
 * @file
 * @brief
 *
 * @date 21.06.2013
 * @author Anton Bondarev
 */

#include <kernel/thread/thread_alloc.h>
#include <mem/page.h>

#include <kernel/thread.h>
#include <mem/misc/pool.h>
#include <assert.h>


#define STACK_SZ      OPTION_GET(NUMBER, thread_stack_size)
static_assert(STACK_SZ > sizeof(struct thread));

#define POOL_SZ       OPTION_GET(NUMBER, thread_pool_size)

typedef union thread_pool_entry {
	struct thread thread;
	char stack[STACK_SZ];
} thread_pool_entry_t __attribute__((aligned(PAGE_SIZE())));

POOL_DEF(thread_pool, thread_pool_entry_t, POOL_SZ);

struct thread *thread_alloc(void) {
	thread_pool_entry_t *block;
	struct thread *t;

	if (!(block = (thread_pool_entry_t *) pool_alloc(&thread_pool))) {
		return NULL;
	}
	memset(block, 0x53, sizeof(*block));

	t = &block->thread;

	thread_stack_init(t, STACK_SZ);

	return t;
}

void thread_free(struct thread *t) {
	thread_pool_entry_t *block;

	assert(t != NULL);

	// TODO may be this is not the best way... -- Eldar
	block = member_cast_out(t, thread_pool_entry_t, thread);
	memset(block, 0xa5, sizeof(*block));

	pool_free(&thread_pool, block);
}
