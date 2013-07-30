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

#ifndef KERNEL_SCHED_SCHED_STRATEGY_H_
#define KERNEL_SCHED_SCHED_STRATEGY_H_

#include <module/embox/kernel/sched/strategy/api.h>

#include <kernel/sched/sched_priority.h>

struct thread;

struct sched_strategy_data; /* Strategy data of thread   */

struct runq;				/* Queue of running threads  */

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
extern void runq_init(struct runq *runq);

#if 0
/**
 * Finilize queue of running threads.
 *
 * @param runq
 *   Running queue.
 */
extern void runq_fini(struct runq *runq);
#endif

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
 * Makes sleep current thread state and adds it to sleeping queue.
 *
 * @param runq
 *   Running queue.
 */
extern void runq_wait(struct runq *runq);


/**
 * Moves thread from sleeping queue to running one and makes it running.
 *
 * @param runq
 * 	 Runninq queue.
 * @param thread
 *   Thread which will be resumed.
 * @retval 0
 *   Switching of current thread is not required.
 * @retval non-zero
 *   Switching of current thread is required.
 */
extern int runq_wake_thread(struct runq *runq, struct thread *thread);

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
		sched_priority_t new_priority);


#endif /* KERNEL_SCHED_SCHED_STRATEGY_H_ */
