/**
 * @file
 *
 * @date Jul 5, 2013
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <kernel/sched.h>

#include <framework/mod/api.h>

#define STACK_SAFE_BOUND 128

extern struct mod __mod__embox__kernel__thread__core;

static inline unsigned int cpu_get_stack(void) {
	unsigned ret;
	__asm__ __volatile__ (
		"mov %%esp, %0\n\t"
		: "=r"(ret)
	);
	return ret;

}

static inline int threads_done(void) {
	return __mod__embox__kernel__thread__core.private->flags & 1;
}

int irq_stack_protection(void) {
	struct thread *th;

	if (!threads_done()) {
		return 0;
	}

	th = sched_current();

	if ((uint32_t) th->stack + STACK_SAFE_BOUND < cpu_get_stack()) {
		return 0;
	}

	return 1;
}
