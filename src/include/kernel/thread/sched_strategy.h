/**
 * @file
 * @brief Defines the simplest operations in scheduler. Implementation depends
 * on the scheduling algorithm. Part of method is used in other functions.
 * All of policy must implement its interface.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_H_
#define KERNEL_THREAD_SCHED_STRATEGY_H_

#include __module_headers(embox/kernel/thread/sched_policy/api)

struct thread;

struct sched_strategy_data;

struct runq;
struct sleepq;

extern void sched_strategy_init(struct sched_strategy_data *data);

extern void runq_init(struct runq *runq, struct thread *current,
		struct thread *idle);

extern struct thread *runq_current(struct runq *runq);

extern int runq_suspend(struct runq *runq, struct thread *thread);
extern int runq_resume(struct runq *runq, struct thread *thread);

extern int sleepq_wake(struct runq *runq, struct sleepq *sleepq, int wake_all);
extern void runq_sleep(struct runq *runq, struct sleepq *sleepq);
extern void runq_unsleep(struct runq *sleepq, struct thread *thread);


extern int runq_change_priority(struct runq *runq, struct thread *thread,
		int new_priority);

extern int runq_switch(struct runq *runq);

extern void sleepq_init(struct sleepq *sleepq);

extern int sleepq_empty(struct sleepq *sleepq);

#define sleepq_foreach(thread, sleepq) \
	  __sleepq_foreach(thread, sleepq)

extern void sleepq_change_priority(struct sleepq *sleepq, struct thread *thread,
		int new_priority);

extern void sleepq_suspend(struct sleepq *sleepq, struct thread *thread);
extern void sleepq_resume(struct sleepq *sleepq, struct thread *thread);

extern struct thread *sleepq_get_thread(struct sleepq *sleepq);

#if 0
/**
 * Initializes all that is necessary for scheduling algorithm.
 * Implementation should properly add the given threads as if both of them has
 * been started using #sched_start().
 *
 * @param current
 *   Thread structure corresponding to the current control flow.
 * @param idle
 *   Thread that should be executed in the last resort.
 * @return
 *   Operation result.
 * @retval 0
 *   If initialization succeeds.
 * @retval non-zero
 *   Error code otherwise.
 */
extern int sched_policy_init(struct thread *current, struct thread *idle);

/**
 * Adds a new thread at scheduler. Should add thread into list of thread
 * which are executed without changing the current thread.
 * @param added_thread
 *   Thread to adding.
 * @return
 *   True when rescheduling is necessary. Otherwise @return false.
 */
extern bool sched_policy_start(struct thread *added_thread);

/**
 * Removes thread from the scheduler. Shouldn't change current thread.
 * If thread doesn't exist in scheduler, there must be NO ERROR.
 * @param removed_thread
 *   Thread to remove.
 * @return
 *  True when rescheduling is necessary. Otherwise @return false.
 */
extern bool sched_policy_stop(struct thread *removed_thread);

/**
 * Switches the current thread pointer to the most appropriate thread. Should
 * change current thread. Threads execution order is implementation dependent.
 * @param prev_thread
 *   Thread which runs now.
 * @return
 *   Most appropriate for the execution thread.
 */
extern struct thread *sched_policy_switch(struct thread *prev_thread);

/**
 * Gets current thread. Shouldn't change current thread between calls of switch
 * @return
 *  Thread that runs now.
 */
extern struct thread *sched_policy_current(void);
#endif

#endif /* KERNEL_THREAD_SCHED_STRATEGY_H_ */
