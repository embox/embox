/**
 * @file
 * @brief Describes light thread structure
 *
 * @date 10.12.2013
 * @author Andrey Kokorev
 * @author Vita Loginova
 */

#ifndef _KERNEL_LTHREAD_H_
#define _KERNEL_LTHREAD_H_


#include <module/embox/kernel/lthread/lthread_api.h>

#include <kernel/sched/schedee.h>
#include <kernel/sched/current.h>
#include <kernel/lthread/lthread_sched_wait.h>
#include <kernel/time/timer.h>

/**
 * lthread is atomic high priority stackless thread. Because of peculiarity of
 * lthread there are some use restrictions.
 *
 * It is impossible to interrupt lthread at the middle of the execution in
 * order to proceed it after some condition becomes true. Nonetheless it is
 * allowed to use waitq and sched_wait functions.
 *
 * For example, you can use WAITQ_WAIT_LTHREAD macro. In case of full thread
 * you are able to allocate memory for waitq_link on the stack. lthread is
 * stackless, so it has to use waitq_link of structure schedee. If conditional
 * expression is not true lthread has to break its routine and wait until it
 * is waken up by waitq. After waking up lthread starts the routine from
 * the first line.
 *
 * So that you should place sync and wait functions at the beginning of the run
 * function in order to execute it in an appropriate context.
 *
 * For waiting and synchronization you can use lthread specific macros and
 * functions (see kernel/lthread/) or macros and functions which are common
 * for all schedee implementation (see kernel/sched/ and
 * kernel/sched/waitq.h and kernel/sched.h).
 *
 * TODO: there is no possibility to use timeout functions yet
 */

struct lthread {
	struct schedee schedee;

	int          (*run)(struct lthread *); /**< Start routine */

	int            label_offset;

	struct sched_wait_info info;
};

#define lthread_self() mcast_out(schedee_get_current(), struct lthread, schedee)

/**
 * Creates a new light thread.
 * @param lt
 *   the light thread to init
 * @param run
 *   the light thread start routine
 */
extern void lthread_init(struct lthread *lt, int (*run)(struct lthread *));

/**
 * Deletes light thread from the pool
 * @param lt
 *   The light thread to delete
 */
extern void lthread_delete(struct lthread *lt);

/**
 * Adds a light thread in runq
 * @param lt
 *   The light thread to launch
 */
extern void lthread_launch(struct lthread *lt);

/**
 * Use case:
 * Helps to go to the right place after waiting
 *
 * static int func(struct lthread *self) {
 *     goto lthread_resume(self, &&initial_label);
 *
 * initial_label:
 *
 *      ... code ...
 *
 * mutex_label:
 *     if (mutex_trylock_lthread(&mutex) == -EAGAIN) {
 *         return lthread_yield(&&initial_label, &&mutex_label);
 *     }
 *
 *     ... code ...
 *
 *     mutex_unlock_lthread(&mutex);
 *
 *     ... code ...
 *
 *     return 0;
 * }
 */

#define lthread_resume(lt, initial_label) \
			*((lt)->label_offset + (initial_label))

#define lthread_yield(initial_label, target_label) \
			(target_label) - (initial_label);

#endif /* _KERNEL_LTHREAD_H_ */
