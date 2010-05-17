/**
 * @file
 * @brief Defines methods to manipulate with threads (scheduler).
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <kernel/thread.h>
#include <kernel/convar.h>
#include <kernel/mutex.h>

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
 * Add a thread to the list of sleeping threads.
 * @param added_thread deleted thread
 * @retval 0 if thread was successfully added.
 * @retval -EINVAL if @c added_thread is NULL or &idle_thread.
 */
int scheduler_convar_wait(struct mutex *added_mutex, struct condition_variable *variable);

/**
 * Transfer threads from sleeping list to list of waiting.
 */
void scheduler_convar_signal(struct condition_variable *variable);


#endif /* SCHEDULER_H_ */
