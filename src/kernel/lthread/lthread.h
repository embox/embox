/**
 * @file
 * @brief Describes light thread structure
 *
 * A light thread is an atomic stackless thread that usually has a high
 * priority. Light threads are very similar to coroutines and have an
 * interface to manage its multiple entry points (see #lthread_resume() and
 * #lthread_yield() functions).
 *
 * Light threads don't have an execution context as opposite to ordinary
 * threads. Nonetheless it is allowed to use waitq and sched_wait functions
 * and share synchronization primitives with regular threads. Of course the
 * absence of the own stack imposes some restrictions, thus one must use the
 * light thread API with care.
 *
 * @note
 * All non-static local variables become invalid when lthread is returned
 * from its run function.
 *
 * Examine the example:
 *
 * @code
 *  static int example(struct lthread *self) {
 *      // Each time lthread runs it is executed from the beginning of its run
 *      // function. If it is necessary to start from another place, use the
 *      // following pattern.
 *      goto lthread_resume(self, &&start);
 *
 *   start:
 *        // If example() runs for the first time, goto jumps here.
 *        ...
 *
 *   mutex_retry:
 *       if (mutex_trylock_lthread(self, &mutex) == -EAGAIN) {
 *           // mutex is held by another schedee, exit from function in order
 *           // to be woken up later. The next time execution will start with
 *           // goto mutex_retry.
 *           return lthread_yield(&&start, &&mutex_retry);
 *       }
 *
 *       ... // executes with mutex held
 *
 *       mutex_unlock_lthread(self, &mutex);
 *
 *       ...
 *
 *       // Return value 0 corresponds to the start label.
 *       return 0;
 *  }
 * @endcode
 *
 * @date 10.12.2013
 * @author Andrey Kokorev
 *          - Initial light thread prototype
 * @author Vita Loginova
 *          - Rewriting from scratch
 */

#ifndef _KERNEL_LTHREAD_H_
#define _KERNEL_LTHREAD_H_

#include <kernel/sched.h>
#include <kernel/lthread/lthread_sched_wait.h>


struct lthread {
	struct schedee schedee;
	int (*run)(struct lthread *); /**< Start routine */
	int label_offset;             /**< Used for calculating label for goto */
	/* Auxiliary structure for waiting with timeout. */
	struct sched_wait_info info;
};

/**
 * Initializes a new light thread.
 * @param lt
 *   The light thread to initialize.
 * @param run
 *   The light thread start routine.
 */
extern void lthread_init(struct lthread *lt, int (*run)(struct lthread *));

/**
 * FIXME: The existing realization isn't good enough since it makes user
 * handle -EAGAIN situation in his own way. Ideologically, this function
 * should make something like #thread_join() (rename it?).
 *
 * Tries to reset the lthread into initial state.
 * @note
 *   It is the user's responsibility to prevent lthread waking up in its run
 *   function.
 * @param lt
 *   The light thread to reset.
 * @return
 *   The operation result.
 * @retval 0
 *   P lt is reset correctly.
 * @retval -EAGAIN
 *   @p lt is in runq or is waiting for an event, try again later.
 */
extern int lthread_reset(struct lthread *lt);

/**
 * Wakes @p lt up or launches it for the first time.
 * @param lt
 *   The light thread to launch.
 */
extern void lthread_launch(struct lthread *lt);

/**
 * Calculates a label offset in order to call #lthread_resume() with
 * @p target_label.
 * @note
 *   This function should be used in pair with return statement.
 * @param initial_label
 *   The beginning label.
 * @param target_label
 *   Label from which lthread should be resumed.
 * @retval
 */
#define lthread_yield(initial_label, target_label) \
			(target_label) - (initial_label);

/**
 * Returns label from which run function should start with.
 * @note
 *   This function should be used in pair with goto statement.
 * @param lt
 *   The current light thread.
 * @param initial_label
 *   The beginning label.
 */
#define lthread_resume(lt, initial_label) \
			*((lt)->label_offset + (initial_label))

#endif /* _KERNEL_LTHREAD_H_ */
