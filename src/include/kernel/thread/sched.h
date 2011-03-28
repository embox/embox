/**
 * @file
 * @brief Defines methods to manipulate with threads (scheduler).
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 * @author Skorodumov Kirill
 */

#ifndef KERNEL_THREAD_SCHED_H_
#define KERNEL_THREAD_SCHED_H_

#include __impl_x(kernel/thread/sched_impl.h)

/**
 * Start working with threads.
 */
extern void sched_start(void);

/**
 * Initializes scheduler.
 */
extern int sched_init(struct thread *current, struct thread *idle);

/**
 * !!!!Can be called only in idle_thread!!!!
 * Stops working with threads. After it contexts can't be switched.
 * Then sched_start can be recalled.
 */
extern void sched_stop(void);

/**
 * Is called after entering a regular critical section.
 * Increases preemption_count.
 */
extern void sched_lock(void);

/**
 * Is called after escaping last critical section.
 * Decreases preemption_count. If the latter one becomes zero,
 * calls sched_dispatch.
 */
extern void sched_unlock(void);

/**
 * TODO
 */
extern void sched_unlock_noswitch(void);

/**
 * TODO
 */
extern void sched_check_switch(void);

/**
 * Changes currently executed thread.
 */
extern void sched_dispatch(void);

/**
 * Adds thread into the list of executable threads.
 * @param added_thread thread to be added into the list
 */
extern void sched_add(struct thread *added_thread);

/**
 * Removes a thread from the list of executable threads.
 * @param removed_thread deleted thread
 * @retval 0 if thread was successfully removed.
 * @retval -EINVAL if @c removed_thread is NULL or &idle_thread.
 */
extern int sched_remove(struct thread *removed_thread);

/**
 *
 * @param thread thread to lock.
 * @param event ???
 * @return 0 if operation executed successfully.
 */
extern int sched_thread_lock(struct thread *thread, struct event *event);

/**
 * puts current thread to sleep
 * adds it to list of given event
 * @param event - event
 */
extern int scher_sleep(struct event *event);

/**
 * wakes up all threads, to given event
 * @param event - event
 */
extern int sched_wakeup(struct event *event);

/**
 * wakes up only first thread in the list of the event
 * @param event - event
 */
extern int sched_wakeup_first(struct event *event);

/**
 * Initialize an event.
 *
 * @param event the initialized event.
 */
extern void event_init(struct event *event);

#endif /* KERNEL_THREAD_SCHED_H_ */
