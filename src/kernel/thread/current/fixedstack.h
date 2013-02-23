/**
 * @file
 * @brief
 *
 * @date 13.02.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_CURRENT_FIXEDSTACK_H_
#define KERNEL_THREAD_CURRENT_FIXEDSTACK_H_

#include <framework/mod/options.h>

#include <types.h>

#include <module/embox/kernel/thread/core.h>

#define THREAD_STACK_SIZE OPTION_MODULE_GET(embox__kernel__thread__core, \
			NUMBER,thread_stack_size)

struct thread;

static inline struct thread *__thread_get_current(void) {
	/*
	 * Thread structure allocated on the top of the stack.
	 * Get current stack pointer and apply mask.
	 */
	return (struct thread *) (((uint32_t)__builtin_frame_address(0))
			& (~(THREAD_STACK_SIZE - 1)));
}

#define __thread_set_current(thread) do { (void) thread; } while (0)

#endif /* KERNEL_THREAD_CURRENT_FIXEDSTACK_H_ */
