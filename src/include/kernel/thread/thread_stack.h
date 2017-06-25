/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_STACK_H_
#define THREAD_STACK_H_

#include <stddef.h>

struct thread;

#include <module/embox/kernel/thread/stack_api.h>

typedef __thread_stack_t thread_stack_t;

extern void thread_stack_init(struct thread *t, size_t size);

extern void thread_stack_set(struct thread *t, void *stack);
extern void thread_stack_set_size(struct thread *t, size_t size);
//extern int thread_stack_reserve(struct thread *t, size_t size);

extern void *thread_stack_get(struct thread *t);

extern size_t thread_stack_get_size(struct thread *t);

extern void * thread_stack_alloc(struct thread *t, size_t size);

#endif /* THREAD_STACK_H_ */
