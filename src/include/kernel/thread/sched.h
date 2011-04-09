/**
 * @file
 * @brief TODO -- Alina.
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 * @author Kirill Skorodumov
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_SCHED_H_
#define KERNEL_THREAD_SCHED_H_

#include __impl_x(kernel/thread/sched_impl.h)

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
 *   Always.
 */
extern int sched_init(struct thread *current, struct thread *idle);

/**
 * Locks the scheduler which means disabling thread switch until
 * #sched_unlock() is called. Each lock must be balanced with the corresponding
 * unlock.
 *
 * @see sched_unlock()
 */
extern void sched_lock(void);

/**
 * Unlocks the scheduler after #sched_lock(). Must be called on the
 * previously locked scheduler only. Outermost unlock (which corresponds to
 * the first lock call) also dispatches pending thread switches (if any).
 *
 * @see sched_lock()
 */
extern void sched_unlock(void);

/**
 * Does the same as #sched_unlock() except that the outermost unlock doesn't
 * perform any special actions.
 */
extern void sched_unlock_noswitch(void);

/**
 * Checks the need to reschedule the current thread and performs the switch
 * if needed.
 */
extern void sched_check_switch(void);

/**
 * Starts scheduling of the given @a thread.
 *
 * @param thread
 *   The thread to be added into the scheduler.
 */
extern void sched_add(struct thread *thread);

/**
 * Removes a @a thread from the scheduler.
 *
 * @param thread
 *   The thread being stopped.
 */
extern void sched_remove(struct thread *thread);

/**
 * Makes the current thread sleep until the specified @a event occurs.
 *
 * @param event
 *   The event to sleep on.
 */
extern int sched_sleep(struct event *event);

extern int sched_sleep_locked(struct event *event);

/**
 * Wakes up all threads that sleep on the given @a event.
 *
 * @param event
 *   The occurred event.
 */
extern int sched_wake(struct event *event);

/**
 * Wakes up one of the threads sleeping on the given @a event.
 *
 * @param event
 *   The occurred event.
 */
extern int sched_wake_one(struct event *event);

#endif /* KERNEL_THREAD_SCHED_H_ */

