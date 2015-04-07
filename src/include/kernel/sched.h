/**
 * @file
 * @brief TODO -- Alina.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 * @author Kirill Skorodumov
 * @author Alina Kramar
 */

#ifndef KERNEL_SCHED_H_
#define KERNEL_SCHED_H_

#include <sys/types.h>

#include <hal/cpu.h>

#include <kernel/sched/sched_lock.h>
#include <kernel/sched/schedee_priority.h>

#include <kernel/time/time.h>

#define SCHED_TIMEOUT_INFINITE     (unsigned long)(-1)

struct schedee;

__BEGIN_DECLS

/**
 * Initializes scheduler.
 *
 * @param current
 *   Schedee structure corresponding to the current control flow.
 * @return
 *   Operation result. At the moment allways success.
 */
extern int sched_init(struct schedee *current);

extern void sched_set_current(struct schedee *schedee);

extern void sched_ticker_init(void);
extern void sched_ticker_fini(void);
extern void sched_ticker_switch(int prev_policy, int next_policy);

extern int sched_active(struct schedee *s);

/**
 * Changes the scheduling priority of the schedee via @p set_priority.
 * If the schedee is ready now (present in the runq), requeues it. If the
 * scehdee's new priority is higher than prioriry of the current schedee,
 * posts rescheduling.
 *
 * @note
 *   For use only in schedee_priortity_*() functions. For setting the new
 *   priority to some schedee use #schedee_priortity_set().
 *
 * @param s
 *   Schedee to operate with.
 * @param prior
 *   New scheduling priority of the schedee.
 * @param set_priority
 *   Function that actually changes a priority value in the corresponging
 *   field of the schedee structure.
 */
extern int sched_change_priority(struct schedee *s, int prior,
		int (*set_priority)(struct schedee_priority *, int));

extern void sched_wait_prepare(void);
extern void sched_wait_cleanup(void);

extern int sched_wakeup(struct schedee *);

/* XXX schedee will not be ever in runq or active - schedee is dead,
 * but with allocated resources on its stack */
extern void sched_freeze(struct schedee *t);

/**
 * Requests switching of the current schedee.
 */
extern void sched_post_switch(void);

/**
 * Runs the scheduler right now.
 */
extern void schedule(void);

extern void sched_finish_switch(struct schedee *prev);
extern void sched_start_switch(struct schedee *next);

/**
 * @brief Makes schedee to run regardless of it's state if schedee is
 * scheduling
 *
 * @param schedee Schedee to operate with
 *
 * @return
 *   Operation result
 * @retval 0
 *   On success.
 * @retval non-zero
 *   On operation fail.
 */
void sched_signal(struct schedee *schedee);

__END_DECLS


#endif /* KERNEL_SCHED_H_ */
