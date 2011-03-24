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
 *  */
extern void sched_policy_init(void);

/**
 * Is called then scheduler start.
 */
extern void sched_policy_start(void);

/**
 * Executes when scheduler stops.
 * Should be done in such way, that allows correctly call then sched_start.
 */
extern void sched_policy_stop(void);

/**
 * Adds a new thread at scheduler. Should add thread into list of thread
 * which are executed without changing the current thread.
 * @param added_thread
 *  thread to adding
 * @return
 *  true when rescheduling is necessary. Otherwise @return false
 */
extern bool sched_policy_add(struct thread *added_thread);

/**
 * Switches the current thread pointer to the most appropriate thread. Should
 * change current thread. Threads execution order is implementation dependent.
 * @param prev_thread
 *  thread which runs now.
 * @return
 *  most appropriate for the execution thread
 */
extern struct thread *sched_policy_switch(struct thread *prev_thread);

/**
 * Removes thread from the scheduler. Shouldn't change current thread.
 * If thread doesn't exist in scheduler, there must be NO ERROR.
 * @param removed_thread
 *  thread to remove
 * @return true when rescheduling is necessary. Otherwise @return false
 */
extern bool sched_policy_remove(struct thread *removed_thread);

/**
 * Gets current thread. Shouldn't change current thread between calls of switch
 * @return
 *  thread that runs now
 */
extern struct thread *sched_policy_current(void);

#endif /* KERNEL_THREAD_SCHED_POLICY_H_ */
