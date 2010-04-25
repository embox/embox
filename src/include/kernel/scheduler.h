/**
 * @file
 * @brief Defines methods to manipulate with threads (scheduler).
 * Implementation look at src/kernel/thread.c.
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/**
 * Initializes threads: set the ability of preemption as true
 * and describes idle_thread.
 */
void scheduler_init(void);

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
 * Is called to change current thread.
 */
void scheduler_dispatch(void);

/**
 * Regularly calls scheduler_dispatch.
 */
void scheduler_tick(uint32_t id);

/**
 * Adds a thread into the list of executable threads.
 */
void scheduler_add (struct thread *added_thread);

/**
 * Adds a thread into the list of executable threads.
 * Returns EINVAL in case of error.
 */
int scheduler_remove (struct thread *removed_thread);

#endif /* SCHEDULER_H_ */
