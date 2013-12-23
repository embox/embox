/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.12.2013
 */

#ifndef SYSCALL_CONTEXT_THREAD_H_
#define SYSCALL_CONTEXT_THREAD_H_

#include <assert.h>
#include <kernel/thread.h>

static inline void syscall_enter(void) {
	struct thread *t = thread_self();

	assert(t);
	assert(t->syscall_ctx_count >= 0);

	++t->syscall_ctx_count;
}

static inline void syscall_leave(void) {
	struct thread *t = thread_self();

	assert(t);
	assert(t->syscall_ctx_count > 0);

	if (!(--t->syscall_ctx_count)) {
		thread_signal_handle();
	}
}

#endif /* SYSCALL_CONTEXT_THREAD_H_ */

