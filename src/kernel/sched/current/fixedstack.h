/**
 * @file
 * @brief
 *
 * @date 13.02.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_SCHEDEE_CURRENT_FIXEDSTACK_H_
#define KERNEL_SCHEDEE_CURRENT_FIXEDSTACK_H_

#include <stdint.h>

#include <framework/mod/options.h>
#include <module/embox/kernel/thread/core.h>

#define THREAD_STACK_SIZE OPTION_MODULE_GET(embox__kernel__thread__core, \
			NUMBER,thread_stack_size)

struct schedee;

static inline struct schedee *__schedee_get_current(void) {
	/*
	 * Thread structure allocated on the top of the stack.
	 * Get current stack pointer and apply mask.
	 */
	return (struct schedee *) (((uint32_t)__builtin_frame_address(0))
			& (~(THREAD_STACK_SIZE - 1)));
}

#define __schedee_set_current(schedee) do { (void) schedee; } while (0)

#endif /* KERNEL_SCHEDEE_CURRENT_FIXEDSTACK_H_ */
