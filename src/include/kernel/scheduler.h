/**
 * @file
 * @brief Defines methods to manipulate with threads (scheduler).
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 * @author Skorodumov Kirill
 */

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <kernel/thread.h>
#include <kernel/mutex.h>
#include <kernel/convar.h>
#include <lib/list.h>


/**
 * Initializes scheduler.
 */
int scheduler_init(void);

/**
 * Start working with threads.
 */
void scheduler_start(void);

/**
 * Is called after entering a regular critical section.
 * Increases preemption_count.
 */
void scheduler_lock(void);

/**
 * Is called after escaping last critical section.
 * Decreases preemption_count. If the latter one becomes zero,
 * calls scheduler_dispatch.
 */
void scheduler_unlock(void);

/**
 * Changes currently executed thread.
 */
void scheduler_dispatch(void);

/**
 * Adds thread into the list of executable threads.
 * @param added_thread thread to be added into the list
 */
void scheduler_add(struct thread *added_thread);

/**
 * Removes a thread from the list of executable threads.
 * @param removed_thread deleted thread
 * @retval 0 if thread was successfully removed.
 * @retval -EINVAL if @c removed_thread is NULL or &idle_thread.
 */
int scheduler_remove(struct thread *removed_thread);

/**
 *
 * @param thread thread to lock.
 * @param event ???
 * @return 0 if operation executed successfully.
 */
int thread_lock(struct thread *thread, struct event *event);

/**
 * puts current thread to sleep
 * adds it to list of given event
 * @param event - event
 */
int scheduler_sleep(struct event *event);

/**
 * wakes up all threads, to given event
 * @param event - event
 */
int scheduler_wakeup(struct event *event);

/**
 * wakes up only first thread in the list of the event
 * @param event - event
 */
int scheduler_wakeup_first(struct event *event);

/**
 * Initialize an event.
 *
 * @param event the initialized event.
 */
void event_init(struct event *event);

#endif /* SCHEDULER_H_ */
