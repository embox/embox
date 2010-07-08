/**
 * @file
 * @brief Defines the simplest operations in scheduler.
 * They are used is a part of other functions.
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#ifndef SCHEDULER_BASE_H_
#define SCHEDULER_BASE_H_

#include <kernel/scheduler.h>

/**
 * Function, executed when thread initializes.
 */
void _scheduler_init(void);

/**
 * Function, executed when thread starts.
 */
void _scheduler_start(void);

/**
 * Add new thread in scheduler.
 */
void _scheduler_add(struct thread *added_thread);

/**
 * Move current_thread pointer to the next thread.
 * @param prev_thread thread, which have worked just now.
 */
struct thread *_scheduler_next(struct thread *prev_thread);

/**
 * Removes thread from the scheduler.
 * @param removed_thread thread to remove.
 */
void _scheduler_remove(struct thread *removed_thread);

#endif /* SCHEDULER_BASE_H_ */
