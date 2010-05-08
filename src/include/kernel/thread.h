/**
 * @file
 * @brief Defines structure of threads and methods,
 * which allows working with them.
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#ifndef _THREAD_H_
#define _THREAD_H_

#include <hal/context.h>
#include <lib/list.h>

typedef int thread_id_t;
typedef int thread_priority_t;
typedef enum {
	THREAD_STATE_RUN,
	THREAD_STATE_WAIT,
	THREAD_STATE_STOP,
	THREAD_STATE_ZOMBIE
} THREAD_STATE;

extern struct thread *idle_thread;

/**
 * Structure, describing threads.
 */
struct thread {
	/** Context of thread. */
	struct context context;
	/** Function, running in thread. */
	void (*run)(void);
	/** List item, corresponding to thread in list of executed threads. */
	struct list_head sched_list;
	/** Flag, which shows, whether tread can be changed. */
	bool reschedule;
	thread_id_t id;
	thread_priority_t priority;
	/** States, which thread can be in. */
    THREAD_STATE state;
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
struct thread *thread_create(void (*run)(void), void *stack_address);

/** Starts a thread.  */
void thread_start(struct thread *thread);

/** Stops chosen thread. */
int thread_stop(struct thread *stopped_thread);

#endif /* THREAD_H_ */
