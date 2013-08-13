/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stdint.h>

#include <kernel/thread/thread_stack.h>
#include <kernel/thread.h>



void thread_stack_init(struct thread *t, size_t size) {
	t->stack.stack = (void *)((uintptr_t)(t) + sizeof(struct thread));
	t->stack.stack_sz = size - sizeof(struct thread);
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

int thread_stack_reserved(struct thread *t, size_t size) {
	if (thread_stack_get_size(t) <= size) {
		return -ENOMEM;
	}

	t->stack.stack_sz += size;

	return ENOERR;
}
