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

void fork_stack_store(struct addr_space *adrspc, struct thread *th) {
	size_t st_size;
	struct stack_space *stspc = NULL, *tmp;

	st_size = thread_stack_get_size(thread_self());

	dlist_foreach_entry(tmp, &adrspc->stack_space_head, list) {
		if (tmp->thread == th) {
			stspc = tmp;
		}
	}

	if (stspc == NULL) {
		stspc = sysmalloc(sizeof(*stspc));
		memset(stspc, 0, sizeof(*stspc));
		stspc->thread = th;
		dlist_init(&stspc->list);
		dlist_add_prev(&stspc->list, &adrspc->stack_space_head);
	}

	if (stspc->stack_sz != st_size) {
		if (stspc->stack) {
			sysfree(stspc->stack);
		}

		stspc->stack = sysmalloc(st_size);
		assert(stspc->stack); /* allocation successed */
		stspc->stack_sz = st_size;
	}

	memcpy(stspc->stack, thread_stack_get(thread_self()), st_size);
}

void fork_stack_restore(struct addr_space *adrspc, void *stack_safe_point) {
	void *stack;
	struct thread *th;
	struct stack_space *stspc = NULL, *tmp;

	th = thread_self();
	stack = thread_stack_get(th);

	dlist_foreach_entry(tmp, &adrspc->stack_space_head, list) {
		if (tmp->thread == th) {
			stspc = tmp;
		}
	}

	if (!stspc)
		return;

	if (stack <= stack_safe_point && stack_safe_point < stack + stspc->stack_sz) {
		off_t off = stack_safe_point - stack;
		size_t sz = stspc->stack_sz - off;

		memcpy(stack + off, stspc->stack + off, sz);
	} else {
		memcpy(stack, stspc->stack, stspc->stack_sz);
	}
}

void fork_stack_cleanup(struct addr_space *adrspc) {
	struct stack_space *tmp;

	dlist_foreach_entry(tmp, &adrspc->stack_space_head, list) {
		if (tmp->stack) {
			sysfree(tmp->stack);
		}

		tmp->stack = NULL;
		tmp->stack_sz = 0;
	}
}

