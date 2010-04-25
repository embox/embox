/**
 * @file
 * @brief Defines structure of threads and methods,
 * working with them. Implementation look at
 * src/kernel/thread.c
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <hal/context.h>

typedef int thread_id_t;
typedef int thread_priority_t;
typedef struct thread * thread_pt;

/**
 * Structure, describing threads.
 */
struct thread {
	struct context thread_context;
	void (*run)(void);
	thread_id_t id;
	thread_priority_t priority;
	bool must_be_switched;
};

/**
 * Creates new thread with function pointer run,
 * stack address stack_address and stack size stack_size.
 * Returns 0 if all parameters are correct and code of error otherwise.
 *
 * @param created_thread
 * @param run
 * @param stack_address
 * @return
 */
int thread_create(struct thread *created_thread, void run(void),
		void *stack_address);

/**
 * Deletes chosen thread.
 */
int thread_delete(struct thread *deleted_thread);

/**
 * Allocates memory for thread _thread.
 * Returns EINVAL, if there is no enough memory for threads.
 * Otherwise returns 0.
 */
struct thread * thread_new(void);


#endif /* THREAD_H_ */
