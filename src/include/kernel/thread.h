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
#include <lib/list.h>

typedef int thread_id_t;
typedef int thread_priority_t;
typedef struct thread * thread_pt;

/**
 * Structure, describing threads.
 */
struct thread {
	/**
	 * Context of thread
	 */
	struct context thread_context;
	/**
	 * Something for mutexes. Maybe. I don't know.)
	 */
	struct thread *next_locked_thread;
	/**
	 * Function, running in thread
	 */
	void (*run)(void);
	/**
	 * List item, corresponding to thread.
	 */
	struct list_head list;
	/**
	 * Flag, which shows, whether tread can be changed.
	 */
	bool must_be_switched;
	thread_id_t id;
	thread_priority_t priority;
};

/**
 * Creates new thread
 *
 * @param created_thread pointer to created thread
 * @param run function executed in created thread
 * @param stack_address address of thread's stack
 * @return 0 if all parameters are correct
 * @return -EINVAL if one of parameters is NULL
 */
int thread_create(struct thread *created_thread, void run(void),
		void *stack_address);

/**
 * Deletes chosen thread.
 */
int thread_delete(struct thread *deleted_thread);

/**
 * Allocates memory for new thread.
 *
 * @return pointer to alloted thread
 * @retval NULL if there are not free threads
 */
struct thread * thread_new(void);


#endif /* THREAD_H_ */
