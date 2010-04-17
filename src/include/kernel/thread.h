/*
 * @file
 * @brief Defines structure of threads and methods,
 * working with them. Implementation look at
 * src/kernel/thread.c
 *
 * @date 16.04.2010
 * @author Avdyukhin Dmitry
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <hal/context.h>

/**
 * Thread, which make nothing.
 * Is used to be working when there is no another process.
 */
extern struct thread * idle_thread;
/**
 * If it doesn't equal to zero, it means
 * that we are located in critical section.
 */
extern int preemption_count;

typedef int thread_id_t;
typedef int thread_priority_t;

/**
 * Structure, describing threads.
 */
struct thread {
	struct context thread_context;
	thread_id_t id;
	thread_priority_t priority;
};

/**
 * Initializes threads: set the ability of preemption as true
 * and describes idle_thread.
 */
void threads_init(void);

/**
 * Creates new thread with function pointer run,
 * stack address stack_address and stack size stack_size.
 */
struct thread *thread_create(void (*run)(void), void *stack_address, size_t stack_size);

/**
 * Deletes chosen thread.
 */
int thread_delete(struct thread *deleted_thread);

/**
 * Is called after entering a regular critical section.
 * Increases preemption_count.
 */
void scheduler_lock(void);

/**
 * Is called after escaping last critical section.
 * Decreases preemption_count. If the latter one becomes zero,
 * calls scheduler_dispatch.
 */
void scheduler_unlock(void);

/**
 * Is called to change current thread.
 */
void scheduler_dispatch(void);


#endif /* THREAD_H_ */
