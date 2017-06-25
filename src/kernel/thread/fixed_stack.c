/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/thread.h>
#include <kernel/thread/thread_stack.h>
#include <stddef.h>

void thread_stack_init(struct thread *t, size_t size) {
	t->stack.stack = t + 1;
	t->stack.stack_sz = size - sizeof *t;
}

void thread_stack_set(struct thread *t, void *stack) {
	t->stack.stack = stack;
}

void *thread_stack_get(struct thread *t) {
	return t->stack.stack;
}

size_t thread_stack_get_size(struct thread *t) {
	return t->stack.stack_sz;
}

void thread_stack_set_size(struct thread *t, size_t size) {
	t->stack.stack_sz = size;
}

#if 0
int thread_stack_reserve(struct thread *t, size_t size) {
	if (size > t->stack.stack_sz) {
		return -ENOMEM;
	}

	t->stack.stack += size;
	t->stack.stack_sz -= size;

	return 0;
}
#endif

void * thread_stack_alloc(struct thread *t, size_t size) {
	void *space;

	if (size > t->stack.stack_sz) {
		return NULL;
	}

	space = t->stack.stack;
	t->stack.stack += size;
	t->stack.stack_sz -= size;

	return space;
}
