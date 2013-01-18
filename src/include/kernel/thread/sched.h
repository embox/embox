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

#include <kernel/thread/types.h>

#define SCHED_TIMEOUT_INFINITE     (unsigned long)(-1)

struct thread;
struct event;
struct sleepq;

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

/**
 * Obtains a pointer to the currently executing thread.
 *
 * @return
 *   The currently executing thread.
 */
extern struct thread *sched_current(void);

/**
 * Makes active thread and adds thread to the queue of ready to executing
 * threads.
 *
 * @param thread
 *   Thread which will be added.
 */
extern void sched_start(struct thread *thread);

/**
 * Makes exit thread and removes thread from scheduler.
 */
extern void sched_finish(struct thread *thread);

/**
 * Makes sleep the current thread and puts it in sleeping queue.
 * Execution is suspended until sched_wake_one() or sched_wake_all()
 * is called.
 *
 * <b>Must not</b> be called with the scheduler @link #sched_lock()
 * locked @endlink or within any other critical context.
 *
 * @param sleepq
 *   Sleeping queue which will be containing thread.
 * @param timeout
 *   Max waiting time of event.
 * @return
 *   Operation result.
 * @retval 0
 *   On success. TODO sleep cancellation is not implemented.
 * @retval SCHED_TIMEOUT_HAPPENED
 *   Timeout happened.
 */
extern int sched_sleep(struct sleepq *sleepq, unsigned long timeout);

/**
 * Does the same as #sched_sleep() but assumes that the scheduler is
 * @link #sched_lock() locked @endlink once. However it still must not be
 * called inside any other critical section or with the scheduler locked more
 * than once.
 *
 * @param sleepq
 *   Sleeping queue which will be containing thread.
 * @param timeout
 *   Max waiting time of event.
 * @return
 *   Operation result.
 * @retval 0
 *   On success. TODO sleep cancellation is not implemented.
 * @retval SCHED_TIMEOUT_HAPPENED
 *   Timeout happened.
 */
extern int sched_sleep_locked(struct sleepq *sleepq, unsigned long timeout);

/**
 * Wakes up one of the threads contained in the given sleeping queue.
 *
 * @param sleepq
 *   The sleeping queue.
 */
extern void sched_wake_one(struct sleepq *sleepq);

/**
 * Wakes up all threads contained in the given sleeping queue.
 *
 * @param sleepq
 *   The sleeping queue.
 */
extern void sched_wake_all(struct sleepq *sleepq);

/**
 * Changes priority of the thread.
 *
 * @param thread
 *   Thread to operate with.
 * @param new_priority
 *   New priority of the the thread.
 */
extern void sched_set_priority(struct thread *thread,
		__thread_priority_t new_priority);

/**
 * Changes scheduling priority of the thread.
 *
 * @param thread
 *   Thread to operate with.
 * @param new_priority
 *   New scheduling priority of the the thread.
 */
extern int sched_change_scheduling_priority(struct thread *t,
		__thread_priority_t new_priority);

/**
 * Returns running time of the thread. To get better precision should be
 * called inside sched_lock().
 *
 * @param thread
 *   Thread
 * @return
 *   Running time in clocks.
 */
extern clock_t sched_get_running_time(struct thread *thread);

/**
 * Requests switching of the current thread.
 */
extern void sched_post_switch(void);

/**
 * @brief Makes thread to run regardless of it's state if thread is scheduling
 * @param thread Thread to operate with
 *
 * @return
 *   Operation result
 * @retval 0
 *   On success.
 * @retval non-zero
 *   On operation fail.
 */
extern int sched_tryrun(struct thread *thread);

#endif /* KERNEL_THREAD_SCHED_H_ */
