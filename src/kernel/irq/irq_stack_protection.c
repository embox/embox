/**
 * @file
 *
 * @date Jul 5, 2013
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include <asm/regs.h>

#include <framework/mod/api.h>

#define STACK_SAFE_BOUND 128

extern struct mod __mod__embox__kernel__thread__core;

static inline int threads_done(void) {
	return mod_is_running(&__mod__embox__kernel__thread__core);
}

int irq_stack_protection(void) {
	struct thread *t;
	struct schedee *schedee = schedee_get_current();

	if (!threads_done() || !schedee) {
		return 0;
	}

	t = thread_self();

	if (thread_stack_get(t) + STACK_SAFE_BOUND < cpu_get_stack()) {
		return 0;
	}

	return 1;
}
