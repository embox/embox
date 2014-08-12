/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    4 Aug 2014
 */

#include "fork_copy_addr_space.h"
#include <kernel/thread.h>
#include <kernel/task/resource/task_fork.h>

#include <sys/types.h>
#include <mem/sysmalloc.h>

void fork_user_stack_store(struct stack_space *stspc, struct thread *thread) {
	size_t st_size;

	st_size = thread_stack_get_size(thread);

	if (stspc->user_stack_sz != st_size) {
		if (stspc->user_stack) {
			sysfree(stspc->user_stack);
		}

		stspc->user_stack = sysmalloc(st_size);
		assert(stspc->user_stack); /* allocation successed */
		stspc->user_stack_sz = st_size;
	}

	memcpy(stspc->user_stack, thread_stack_get(thread), st_size);
}

void fork_user_stack_restore(struct stack_space *stspc, struct thread *th, void *stack_safe_point) {
	void *stack = thread_stack_get(th);

	if (stack <= stack_safe_point && stack_safe_point < stack + stspc->user_stack_sz) {
		off_t off = stack_safe_point - stack;
		size_t sz = stspc->user_stack_sz - off;

		memcpy(stack + off, stspc->user_stack + off, sz);
	} else {
		memcpy(stack, stspc->user_stack, stspc->user_stack_sz);
	}
}

void fork_user_stack_cleanup(struct stack_space *stspc) {
	if (stspc->user_stack) {
		sysfree(stspc->user_stack);
	}

	stspc->user_stack = NULL;
	stspc->user_stack_sz = 0;
}

