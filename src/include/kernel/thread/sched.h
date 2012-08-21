/**
 * @file
 * @brief TODO -- Alina.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 * @author Kirill Skorodumov
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_SCHED_H_
#define KERNEL_THREAD_SCHED_H_

#include <kernel/thread/sched_lock.h>
#include <kernel/thread/sched_priority.h>

#include __impl_x(kernel/thread/sched_impl.h)
#include __impl_x(kernel/thread/types.h)

#define SCHED_TIMEOUT_INFINITE     (unsigned long)(-1)

#define SCHED_SLEEP_TIMEOUT        1
#define SCHED_SLEEP_INTERRUPT      3

struct thread;
struct event;

/**
 * Initializes scheduler.
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
 *   Error code if initialization of the underlying policy implementation has
 *   failed.
 */
extern int sched_init(struct thread *current, struct thread *idle);

extern struct thread *sched_current(void);

extern void sched_set_priority(struct thread *thread,
		__thread_priority_t new_priority);

/**
 * Makes the current thread sleep until the specified @a event occurs.
 * Execution is suspended until #sched_wake() or #sched_wake_one() is called on
 * the @a event.
 *
 * <b>Must not</b> be called with the scheduler @link #sched_lock()
 * locked @endlink or within any other critical context.
 *
 * @param event
 *   The event to sleep on.
 * @param timeout
 *   Max waiting time of event.
 * @return
 *   Operation result.
 * @retval 0
 *   On success. TODO sleep cancellation is not implemented.
 * @retval SCHED_TIMEOUT_HAPPENED
 *   Timeout happened.
 */
extern int sched_sleep(struct event *event, unsigned long timeout);

/**
 * @brief Makes thread to run regardless of it's state
 * @param thread Thread to operate with
 *
 * @return
 *   Operation result
 * @retval 0
 *   On success.
 * @retval non-zero
 *   On operation fail.
 */
extern int sched_setrun(struct thread *thread);

/**
 * Wakes up all threads that sleep on the given @a event.
 *
 * @param event
 *   The occurred event.
 */
extern void sched_wake(struct event *event);

/**
 * Moves the current thread to the end of the queue for its priority.
 */
extern void sched_yield(void);

extern void sched_suspend(struct thread *thread);

extern void sched_resume(struct thread *thread);


extern int sched_change_scheduling_priority(struct thread *t,
		__thread_priority_t new);

/**
 * Wakes up one of the threads sleeping on the given @a event.
 *
 * @param event
 *   The occurred event.
 */
extern void sched_wake_one(struct event *event);

/**
 * Does the same as #sched_sleep() but assumes that the scheduler is
 * @link #sched_lock() locked @endlink once. However it still must not be
 * called inside any other critical section or with the scheduler locked more
 * than once.
 *
 * @param event
 *   The event to sleep on.
 * @param timeout
 *   Max waiting time of event.
 * @return
 *   Operation result.
 * @retval 0
 *   On success. TODO sleep cancellation is not implemented.
 * @retval SCHED_TIMEOUT_HAPPENED
 *   Timeout happened.
 */
extern int sched_sleep_locked(struct event *event, unsigned long timeout);


#endif /* KERNEL_THREAD_SCHED_H_ */
