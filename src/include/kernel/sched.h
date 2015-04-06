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
#include <kernel/sched/sched_priority.h>

#include <kernel/time/time.h>

#define SCHED_TIMEOUT_INFINITE     (unsigned long)(-1)

struct schedee;

__BEGIN_DECLS

/**
 * Initializes scheduler.
 *
 * @param current
 *   Thread structure corresponding to the current control flow.
 * @return
 *   Operation result.
 * @retval 0
 *   If initialization succeeds.
 * @retval non-zero
 *   Error code if initialization of the underlying policy implementation has
 *   failed.
 */
extern int sched_init(struct schedee *current);

extern void sched_set_current(struct schedee *schedee);

extern void sched_ticker_init(void);
extern void sched_ticker_fini(void);
extern void sched_ticker_switch(int prev_policy, int next_policy);

extern void sched_sleep(void);

extern int sched_active(struct schedee *s);

/**
 * Changes scheduling priority of the thread. If the thread is running now
 * (present in the runq) calls runq_change_priority() for rescheduling runq
 * and thread_set_priority() else just calls thread_set_priority() for setup
 * a new value for priority field in sched_strategy_data structure
 *
 * @param t
 *   Thread to operate with.
 * @param priority
 *   New scheduling priority of the the thread.
 */
extern int sched_change_priority(struct schedee *s, sched_priority_t prior,
		int (*set_priority)(struct schedee_priority *, sched_priority_t));

extern void sched_wait_prepare(void);
extern void sched_wait_cleanup(void);

extern int __sched_wakeup(struct schedee *);
extern int sched_wakeup(struct schedee *);

/* XXX thread will not be ever in runq or active - thread is dead,
 * but with allocated resources on its stack */
extern void sched_freeze(struct schedee *t);

/**
 * Requests switching of the current thread.
 */
extern void sched_post_switch(void);

/**
 * Runs the scheduler right now.
 */
extern void schedule(void);

extern void sched_finish_switch(struct schedee *prev);
extern void sched_start_switch(struct schedee *next);

/**
 * @brief Makes schedee to run regardless of it's state if thread is scheduling
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
