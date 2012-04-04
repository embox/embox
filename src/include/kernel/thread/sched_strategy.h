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
 * Initializes queue of running threads. Resumes current and idle threads.
 * Current and idle threads must be suspended.
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
 * Returns current executing thread.
 * @param runq
 *   Running queue.
 */
extern struct thread *runq_current(struct runq *runq);

/**
 * Suspends thread from running queue. Thread must be resumed.
 * @param runq
 *   Running queue.
 * @param thread
 *   Thread which will be resumed.
 * @retval 0
 *   Switching of current thread is not required.
 * @retval non-zero
 *   Switching of current thread is required.
 */
extern int runq_suspend(struct runq *runq, struct thread *thread);

/**
 * Resumes thread which is not sleeping. Thread must be suspended.
 * @param runq
 *   Running queue.
 * @param thread
 *   Thread which will be resumed.
 * @retval 0
 *   Switching of current thread is not required.
 * @retval non-zero
 *   Switching of current thread is required.
 */
extern int runq_resume(struct runq *runq, struct thread *thread);

/**
 * Wakes up threads from sleeping queue and adds them to running queue.
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
 * Makes sleep current thread and adds it to sleeping queue.
 *
 * @param runq
 *   Running queue.
 * @param sleepq
 *   Sleeping queue.
 */
extern void runq_sleep(struct runq *runq, struct sleepq *sleepq);

/**
 * Wakes up resumed sleeping thread and adds it to running queue.
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
extern int sleepq_wake_resumed_thread(struct runq *runq, struct sleepq *sleepq, struct thread *thread);

/**
 * Wakes up suspended sleeping thread.
 *
 * @param sleepq
 *   Sleeping queue.
 * @param thread
 *   Thread which will be resumed.
 */
extern void sleepq_wake_suspended_thread(struct sleepq *sleepq, struct thread *thread);


/**
 * Sets priority of running thread.
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
 * Switches current thread.
 *
 * @param runq
 *   Running queue.
 * @retval 0
 *   Current thread is not switched.
 * @retval 0
 *   Current thread is switched.
 */
extern int runq_switch(struct runq *runq);

/**
 * Initializes queue of sleeping threads.
 *
 * @param sleepq
 *   Sleeping queue.
 */
extern void sleepq_init(struct sleepq *sleepq);

/**
 * !!!
 *
 * @param sleepq
 *   Sleeping queue.
 * @retval
 *
 */
extern int sleepq_empty(struct sleepq *sleepq);

#define sleepq_foreach(thread, sleepq) \
	  __sleepq_foreach(thread, sleepq)

/**
 * Sets priority of sleeping thread.
 *
 * @param sleepq
 *   Sleeping queue.
 * @param thread
 *   Thread from sleeping thread.
 * @param new_priority
 *   New priority.
 */
extern void sleepq_change_priority(struct sleepq *sleepq, struct thread *thread,
		int new_priority);

/**
 * Suspends sleeping thread. Thread must be resumed.
 *
 * @param sleepq
 *   Sleeping queue.
 * @param thread
 *   Thread which will be suspended.
 */
extern void sleepq_suspend(struct sleepq *sleepq, struct thread *thread);

/**
 * Resumes sleeping thread. Thread must be suspended.
 *
 * @param sleepq
 *   Sleeping queue.
 * @param thread
 *   Thread which will be resumed.
 */
extern void sleepq_resume(struct sleepq *sleepq, struct thread *thread);

/**
 * !!!
 *
 * @param sleepq
 *
 * @retval
 *
 */
extern struct thread *sleepq_get_thread(struct sleepq *sleepq);

#endif /* KERNEL_THREAD_SCHED_STRATEGY_H_ */

