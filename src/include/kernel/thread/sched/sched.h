/**
 * @file
 * @brief Defines methods to manipulate with threads (scheduler).
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 * @author Skorodumov Kirill
 */

// XXX
#include <stdbool.h>
#include <kernel/thread/thread.h>

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/**
 * Start working with threads.
 */
extern void scheduler_start(void);

/**
 * !!!!Can be called only in idle_thread!!!!
 * Stops working with threads. After it contexts can't be switched.
 * Then scheduler_start can be recalled.
 */
extern void scheduler_stop(void);

/**
 * Is called after entering a regular critical section.
 * Increases preemption_count.
 */
extern void scheduler_lock(void);

/**
 * Is called after escaping last critical section.
 * Decreases preemption_count. If the latter one becomes zero,
 * calls scheduler_dispatch.
 */
extern void scheduler_unlock(void);

/**
 * Changes currently executed thread.
 */
extern void scheduler_dispatch(void);

/**
 * Adds thread into the list of executable threads.
 * @param added_thread thread to be added into the list
 */
extern void scheduler_add(struct thread *added_thread);

/**
 * Removes a thread from the list of executable threads.
 * @param removed_thread deleted thread
 * @retval 0 if thread was successfully removed.
 * @retval -EINVAL if @c removed_thread is NULL or &idle_thread.
 */
extern int scheduler_remove(struct thread *removed_thread);

/**
 *
 * @param thread thread to lock.
 * @param event ???
 * @return 0 if operation executed successfully.
 */
extern int thread_lock(struct thread *thread, struct event *event);

/**
 * puts current thread to sleep
 * adds it to list of given event
 * @param event - event
 */
extern int scheduler_sleep(struct event *event);

/**
 * wakes up all threads, to given event
 * @param event - event
 */
extern int scheduler_wakeup(struct event *event);

/**
 * wakes up only first thread in the list of the event
 * @param event - event
 */
extern int scheduler_wakeup_first(struct event *event);

/**
 * Initialize an event.
 *
 * @param event the initialized event.
 */
extern void event_init(struct event *event);

extern struct thread *scheduler_current(void);

#endif /* SCHEDULER_H_ */
