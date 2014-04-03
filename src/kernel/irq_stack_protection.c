/**
 * @file
 *
 * @date Jul 5, 2013
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include <framework/mod/api.h>

#define STACK_SAFE_BOUND 128

extern struct mod __mod__embox__kernel__thread__core;

static inline void *cpu_get_stack(void) {
	void * ret;
	__asm__ __volatile__ (
		"mov %%esp, %0\n\t"
		: "=r"(ret)
	);
	return ret;

}

static inline int threads_done(void) {
	return __mod__embox__kernel__thread__core.priv->flags & 1;
}

int irq_stack_protection(void) {
	struct thread *t;

	if (!threads_done()) {
		return 0;
	}

	t = thread_self();

	if (thread_stack_get(t) + STACK_SAFE_BOUND < cpu_get_stack()) {
		return 0;
	}

	return 1;
}
