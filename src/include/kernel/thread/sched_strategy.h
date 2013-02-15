/**
 * @file
 * @brief Defines the simplest operations in scheduler. Implementation depends
 * on the scheduling algorithm. Part of method is used in other functions.
 * All of policy must implement its interface.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 * @author Bulychev Anton
 *          - Description of functions
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_H_
#define KERNEL_THREAD_SCHED_STRATEGY_H_

#include <module/embox/kernel/thread/sched_policy/api.h>

struct thread;

struct sched_strategy_data; /* Strategy data of thread   */

struct runq;				/* Queue of running threads  */
struct sleepq;				/* Queue of sleeping threads */

/**
 * Initializes scheduler strategy data of the thread.
 *
 * @param data
 *   Structure where storing strategy data of thread.
 */
extern void sched_strategy_init(struct sched_strategy_data *data);

/**
 * Initializes queue of running threads. Makes activate current and idle
 * threads. Current and idle threads must be inactive.
 *
 * @param runq
 *   Running queue.
 * @param current
 *   Thread which will be first.
 * @param idle
 *   Idle thread.
 */
extern void runq_init(struct runq *runq, struct thread *current,
		struct thread *idle);

/**
 * Finilize queue of running threads.
 *
 * @param runq
 *   Running queue.
 */
extern void runq_fini(struct runq *runq);

/**
 * Initializes queue of sleeping threads.
 *
 * @param sleepq
 *   Sleeping queue.
 */
extern void sleepq_init(struct sleepq *sleepq);

/**
 * Returns pointer to the currently executing thread.
 *
 * @param runq
 *   Running queue.
 */
extern struct thread *runq_current(struct runq *runq);

/**
 * Makes activate thread state and puts it in running queue.
 *
 * @param runq
 *   Running queue.
 * @param thread
 *   Thread which will be started.
 * @retval 0
 *   Switching of current thread is not required.
 * @retval non-zero
 *   Switching of current thread is required.
 */
extern int runq_start(struct runq *runq, struct thread *thread);

/**
 * Makes exit thread state and removes from running queue.
 *
 * @param runq
 *   Running queue.
 * @param thread
 *   Thread which will be finished.
 * @retval 0
 *   Switching of current thread is not required.
 * @retval non-zero
 *   Switching of current thread is required.
 */
extern int runq_finish(struct runq *runq, struct thread *thread);

/**
 * Makes exit thread state and removes from sleeping queue.
 *
 * @param sleepq
 *   Sleeping queue.
 * @param thread
 *   Thread which will be finished.
 */
extern void sleepq_finish(struct sleepq *sleepq, struct thread *thread);

/**
 * Makes sleep current thread state and adds it to sleeping queue.
 *
 * @param runq
 *   Running queue.
 * @param sleepq
 *   Sleeping queue.
 */
extern void runq_sleep(struct runq *runq, struct sleepq *sleepq);

/**
 * Moves threads from sleeping queue to running one and makes running their states.
 * If wake_all = 0 then wakes up only one thread from sleepq, else wakes up
 * all threads.
 *
 * @param runq
 *   Running queue.
 * @param sleepq
 *   Sleeping queue.
 * @param wake_all
 *   Waking all threads or only one.
 * @retval 0
 *   Switching of current thread is not required.
 * @retval non-zero
 *   Switching of current thread is required.
 */
extern int sleepq_wake(struct runq *runq, struct sleepq *sleepq, int wake_all);

/**
 * Moves thread from sleeping queue to running one and makes it running.
 *
 * @param runq
 * 	 Runninq queue.
 * @param sleepq
 *   Sleeping queue.
 * @param thread
 *   Thread which will be resumed.
 * @retval 0
 *   Switching of current thread is not required.
 * @retval non-zero
 *   Switching of current thread is required.
 */
extern int sleepq_wake_thread(struct runq *runq, struct sleepq *sleepq, struct thread *thread);

/**
 * Switches current thread.
 *
 * @param runq
 *   Running queue.
 * @retval
 *   Thread to perform next. May equals to current.
 */
extern struct thread *runq_switch(struct runq *runq);

/**
 * Sets scheduling priority of running thread.
 *
 * @param runq
 *   Running queue.
 * @param thread
 *   Thread from running queue.
 * @param new_priority
 *   New priority.
 * @retval 0
 *   Switching of current thread is not required.
 * @retval non-zero
 *   Switching of current thread is required.
 */
extern int runq_change_priority(struct runq *runq, struct thread *thread,
		int new_priority);

/**
 * Sets scheduling priority of sleeping thread.
 *
 * @param sleepq
 *   Sleeping queue.
 * @param thread
 *   Thread from sleeping queue.
 * @param new_priority
 *   New priority.
 */
extern void sleepq_change_priority(struct sleepq *sleepq, struct thread *thread,
		int new_priority);

/**
 * Returns is empty queue.
 *
 * @param sleepq
 *   Sleeping queue.
 * @retval 0
 *   Queue is not empty.
 * @retval non-zero
 *   Queue is empty.
 */
extern int sleepq_empty(struct sleepq *sleepq);

/**
 * Returns pointer of first thread in sleeping queue
 *
 * @param sleepq
 *   Sleeping queue.
 * @retval
 *   First thread.
 */
extern struct thread *sleepq_get_thread(struct sleepq *sleepq);

#define sleepq_foreach(thread, sleepq) \
	  __sleepq_foreach(thread, sleepq)

#endif /* KERNEL_THREAD_SCHED_STRATEGY_H_ */
