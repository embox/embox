/**
 * @file
 * @brief Defines the simplest operations in scheduler. Implementation depends
 * on the scheduling algorithm. Part of method is used in other functions.
 * All of policy must implement its interface.
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 */

#ifndef KERNEL_THREAD_SCHED_POLICY_H_
#define KERNEL_THREAD_SCHED_POLICY_H_

#include <stdbool.h>
#include __impl_x(kernel/thread/types.h)

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

#endif /* KERNEL_THREAD_SCHED_POLICY_H_ */
