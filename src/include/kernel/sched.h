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
#include <kernel/sched/sched_timing.h>
#include <kernel/sched/affinity.h>
#include <kernel/sched/schedee_priority.h>

#include <kernel/sched/waitq.h>
#include <kernel/sched/runq.h>

#include <kernel/time/time.h>

#define SCHED_TIMEOUT_INFINITE     (-1L)

#ifdef SMP
# define TW_SMP_WAKING  (~0x0)  /**< In the middle of sched_wakeup. */
#else
# define TW_SMP_WAKING  (0x0)   /**< Not used in non-SMP kernel. */
#endif

enum schedee_type {
	SCHEDEE_THREAD,
	SCHEDEE_LTHREAD,
};

/**
 * The abstract structure for scheduling entities.
 *
 * Locking:
 *   s->lock    - used during waking up
 *   s->active  - (SMP) only current is allowed to modify it,
 *                reads are usually paired with s->waiting
 *   s->ready   - any access must be protected with rq lock and interrupts
 *                off, only current can reset it to zero (during 'schedule'),
 *                others can set it to a non-zero during wake up
 *   s->waiting - current can change it from zero to a non-zero with no locks,
 *                others access it with s->lock held and interrupts off
 */
struct schedee {
	runq_item_t       runq_link;

	spinlock_t        lock; /**< Protects wait state and others. */

	enum schedee_type type; /** < Thread or lthread */

	/**
	 * Process function is called in the function schedule() after extracting
	 * the next schedee from the runq. This function performs all necessary
	 * actions with a specific schedee implementation.
	 *
	 * It has to enable ipl as soon as possible.
	 *
	 * Returns schedee to which context switched.
	 */
	struct schedee    *(*process)(struct schedee *prev, struct schedee *next);

	/* Fields corresponding to the state in the scheduler state machine. */
	unsigned int active;  /**< Running on a CPU. TODO SMP-only. */
	unsigned int ready;   /**< Managed by the scheduler. */
	unsigned int waiting; /**< Waiting for an event. */

	struct affinity         affinity;
	struct sched_timing     sched_timing;
	struct schedee_priority priority;

	struct waitq_link waitq_link; /**< Used as a link in different waitqs. */
};

__BEGIN_DECLS

static inline int schedee_is_thread(struct schedee *s) {
	return s->type == SCHEDEE_THREAD;
}

/**
 * Initializes the scheduler.
 *
 * @param current
 *   The schedee structure corresponding to the current control flow.
 * @return
 *   The operation result. At the moment always success.
 */
extern int sched_init(struct schedee *current);

/**
 * Initializes @p schedee.
 *
 * @param  schedee  The schedee to initialize.
 * @param  priority The schedee priority in the runq.
 * @param  process  The schedee proces function. See #schedee for details.
 * @return
 *   The operation result. At the moment always success.
 */
extern int schedee_init(struct schedee *schedee, int priority,
	struct schedee *(*process)(struct schedee *prev, struct schedee *next),
	enum schedee_type type);

extern void sched_set_current(struct schedee *schedee);

extern void sched_ticker_add(unsigned int cpuid);
extern void sched_ticker_del(unsigned int cpuid);

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
 * @param schedee
 *   Schedee to operate with.
 * @param prior
 *   New scheduling priority of the schedee.
 * @param set_priority
 *   Function that actually changes a priority value in the corresponding
 *   field of the schedee structure.
 */
extern int sched_change_priority(struct schedee *schedee, int prior,
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
