/**
 * @file
 * @brief Defines the simplest operations in scheduler.
 * They are used is a part of other functions.
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 */

#ifndef KERNEL_THREAD_SCHED_LOGIC_H_
#define KERNEL_THREAD_SCHED_LOGIC_H_

#include <stdbool.h>
#include __impl_x(kernel/thread/types.h)

/**
 * Function, executed when thread initializes.
 */
extern void sched_policy_init(void);

/**
 * Function, executed when thread starts.
 */
extern void sched_policy_start(void);

/**
 * Function, executed when scheduler stops.
 * Must be made in such way, that allows correctly call then sched_start.
 */
extern void sched_policy_stop(void);

/**
 * Adds new thread in thread list.
 * @param added_thread
 * @return true when rescheduling is necessary
 */
extern bool sched_policy_add(struct thread *added_thread);

/**
 * Move current_thread pointer to the next thread.
 * @param prev_thread thread, which have worked just now.
 */
extern struct thread *sched_policy_switch(struct thread *prev_thread);

/**
 * Removes thread from the scheduler.
 * If thread doesn't exist in scheduler, there must be NO ERROR.
 * @param removed_thread thread to remove.
 * @return true when rescheduling is necessary
 */
extern bool sched_policy_remove(struct thread *removed_thread);

/**
 * Gets current thread.
 * @return thread that runs now
 */
extern struct thread *sched_policy_current(void);

#endif /* KERNEL_THREAD_SCHED_LOGIC_H_ */
