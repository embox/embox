/*
 * @file thread.h
 * @brief Defines structure of threads and methods,
 * working with them. Implementation look at
 * src/kernel/thread.c
 *
 * @date 16.04.2010
 * @author Avdyukhin Dmitry
 */

#include <hal/context.h>


#ifndef THREAD_H_
#define THREAD_H_

static struct thread * idle_thread;
static int preemption_count;

typedef int id_t;
typedef int priority_t;

struct thread {
	struct context thread_context;
	id_t id;
	priority_t priority;
};

struct thread * thread_create(void (*run)(void), void *stack_address, size_t stack_size);
void thread_delete(struct thread * deleted_thread);
void idle_run(void);

void scheduler_lock(void);
void scheduler_unlock(void);
void scheduler_dispatch(void);


#endif /* THREAD_H_ */
