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
 *      goto *lthread_resume(self, &&start);
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

#include <stddef.h>
#include <stdbool.h>

#include <kernel/sched.h>
#include <kernel/lthread/lthread_sched_wait.h>


struct lthread {
	struct schedee schedee;
	int (*run)(struct lthread *); /**< Start routine */
	ptrdiff_t label_offset;       /**< Used for calculating label for goto */
	/* Auxiliary structure for waiting with timeout. */
	struct sched_wait_info info;
	struct schedee *joining; /**< joining schedee with stack */
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
 * Waits for @p lt is not waiting or scheduled and after that resets @p lt
 * into initial state. @p lt can be safely launched after executing this
 * function.
 * @note
 *   Can be used only by schedee with stack.
 * @note
 *   It is the user's responsibility to prevent lthread waking up in its run
 *   function.
 * @param lt
 *   The light thread to join.
 */
extern void lthread_join(struct lthread *lt);

/**
 * Wakes @p lt up or launches it for the first time.
 * @param lt
 *   The light thread to launch.
 */
extern void lthread_launch(struct lthread *lt);

/**
 * Calculates a label offset in order to call #lthread_resume() with
 * @p target_lbl.
 * @note
 *   This function should be used in pair with return statement.
 * @param initial_lbl
 *   The beginning label.
 * @param target_lbl
 *   Label from which lthread should be resumed.
 */
static inline ptrdiff_t lthread_yield(void *initial_lbl, void *target_lbl) {
	return target_lbl - initial_lbl;
}

/**
 * Returns label from which run function should start with.
 * @note
 *   This function should be used in pair with goto statement.
 * @param lt
 *   The current light thread.
 * @param initial_lbl
 *   The beginning label.
 */
static inline void *lthread_resume(struct lthread *lt, void *initial_lbl) {
	return initial_lbl + lt->label_offset;
}

extern struct schedee *lthread_process(struct schedee *prev,
		struct schedee *next);

#define LTHREAD_DEF(_lth, _run, _prio) \
	struct lthread _lth = { \
		.run = _run, \
		.schedee = { \
			.runq_link = RUNQ_ITEM_INIT(_lth.schedee.runq_link), \
			.lock = SPIN_UNLOCKED, \
			.process = lthread_process, \
			.ready = false, \
			.active = false, \
			.waiting = true, \
			.affinity = SCHED_AFFINITY_INIT(), \
			.priority = SCHED_PRIORITY_INIT(_prio), \
			.sched_timing = SCHED_TIMING_INIT(), \
		} \
	}

#endif /* _KERNEL_LTHREAD_H_ */
