/**
 * @file
 *
 * @brief Interface of periodical timers with milliseconds precision.
 *
 * @details
 *   for set timer use `sys_timer_init_start' or `sys_timer_set' functions.
 *   for emulate non-periodical behavior use sys_timer_close function in the end of handler.
 *
 * @date 20.07.10
 * @author Fedor Burdun
 * @author Ilia Vaprol
 */

#ifndef KERNEL_TIME_SYS_TIMER_H_
#define KERNEL_TIME_SYS_TIMER_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <hal/cpu.h>
#include <hal/ipl.h>

#include <kernel/time/timer_strat.h>
#include <util/atomic_rmw.h>

__BEGIN_DECLS

struct sys_timer;

/**
 * Type of timer's handler. Function that will be called by timers every n-th
 * milliseconds.
 */
typedef void (*sys_timer_handler_t)(struct sys_timer *timer, void *param);

#define SYS_TIMER_PERIODIC       0x1
#define SYS_TIMER_ONESHOT        0x0

#define SYS_TIMER_STATE_PREALLOC 0x1
#define SYS_TIMER_STATE_STARTED  0x2

struct sys_timer_sharing {
	uint8_t is_shared;
	/* Shared on multiprocessor, each bit indicate a CPU */
	uint32_t shared_cpu;
};

/**
 * system timer structure
 */
struct sys_timer {
	sys_timer_queue_t lnk;
#ifdef SMP /* XXX */
	sys_timer_queue_t multi_lnk[NCPU];
	/**
	 * One counter per list, for the same reason as one link per list.
	 *
	 * The lists are delta lists, so a queued timer's counter means "after the
	 * entry in front of me, in this list". A timer on four lists needs four of
	 * them. Only the scheduler ticker is; every other timer uses `cnt` below
	 * and leaves this alone. See TIMER_CNT() in head_timer_smp.c.
	 */
	clock_t multi_cnt[NCPU];
	struct sys_timer_sharing *timer_sharing;
	/**
	 * Which CPU's list this timer is queued on.
	 *
	 * The timer lists are per-CPU, and a timer outlives the caller's stay on a
	 * CPU -- a timed wait routinely starts its timer on one core and closes it
	 * on another. Written by timer_strat_start(), read by timer_strat_stop().
	 * -1 when not queued. The scheduler ticker is on every list at once and
	 * does not use this.
	 */
	int owner_cpu;
#endif
	struct dlist_head st_wait_link;

	uint32_t load;
	/* Clocks count at which timer should fire: cnt = current clocks() + load. */
	clock_t cnt;
	sys_timer_handler_t handle;
	void *param;
	unsigned int flags;
	uint32_t state; /**< do we use sys_timer_set or timer_init_start? */
};

static inline bool sys_timer_is_preallocated(struct sys_timer *tmr) {
	return tmr->state & SYS_TIMER_STATE_PREALLOC;
}

static inline void sys_timer_set_preallocated(struct sys_timer *tmr) {
	tmr->state |= SYS_TIMER_STATE_PREALLOC;
}

static inline void sys_timer_clear_preallocated(struct sys_timer *tmr) {
	tmr->state &= ~SYS_TIMER_STATE_PREALLOC;
}

#ifdef SMP /* XXX */
static inline bool sys_timer_is_shared(struct sys_timer *tmr) {
	if(tmr->timer_sharing) return tmr->timer_sharing->is_shared;
	else return 0;
}

static inline void sys_timer_set_shared(struct sys_timer *tmr) {
	if(tmr->timer_sharing) tmr->timer_sharing->is_shared = 1;
}

static inline void sys_timer_set_private(struct sys_timer *tmr) {
	if(tmr->timer_sharing) tmr->timer_sharing->is_shared = 0;
}
#endif

/* The mask stays up for the whole answer, and both words are read atomically.
 * `cpu_get_id()` names the bit; reading it and then using it after
 * ipl_restore() asks about a core this thread may no longer be on. */
static inline bool sys_timer_is_started(struct sys_timer *tmr) {
#ifdef SMP /* XXX */
	bool ret;
	ipl_t ipl = ipl_save();
	unsigned int cpuid = cpu_get_id();

	if (!sys_timer_is_shared(tmr)) {
		ret = !!(atomic_rmw_load(&tmr->state, __ATOMIC_RELAXED)
		         & SYS_TIMER_STATE_STARTED);
	}
	else if (!(atomic_rmw_load(&tmr->state, __ATOMIC_RELAXED)
	           & SYS_TIMER_STATE_STARTED)) {
		ret = 0;
	}
	else {
		ret = !!(atomic_rmw_load(&tmr->timer_sharing->shared_cpu, __ATOMIC_RELAXED)
		         & (0x1u << cpuid));
	}
	ipl_restore(ipl);

	return ret;
#else
	return tmr->state & SYS_TIMER_STATE_STARTED;
#endif
}

static inline void sys_timer_set_started(struct sys_timer *tmr) {
#ifdef SMP /* XXX */
	ipl_t ipl = ipl_save();
	unsigned int cpuid = cpu_get_id();

	if (sys_timer_is_shared(tmr)) {
		/* Every core writes this word. A plain |= loses another core's
		 * concurrent &=, and a lost clear is what makes a later
		 * timer_strat_stop() mend a list this timer is not on.
		 *
		 * Relaxed, not a shortcut: a core only ever writes its own bit and
		 * reads it only to decide about its own list, so nothing here orders
		 * anything else -- atomicity alone is what is needed. Sequential
		 * consistency would be a barrier on every context switch, 15% of
		 * `smptest malloc 4`, measured. */
		atomic_rmw_or_fetch(&tmr->timer_sharing->shared_cpu, 0x1u << cpuid,
		    __ATOMIC_RELAXED);
	}
	atomic_rmw_or_fetch(&tmr->state, SYS_TIMER_STATE_STARTED, __ATOMIC_RELAXED);
	ipl_restore(ipl);
#else
	tmr->state |= SYS_TIMER_STATE_STARTED;
#endif
}

static inline void sys_timer_set_stopped(struct sys_timer *tmr) {
#ifdef SMP /* XXX */
	ipl_t ipl = ipl_save();
	unsigned int cpuid = cpu_get_id();

	if (sys_timer_is_shared(tmr)) {
		/* and_fetch, not fetch_and: the decision below is about the word as
		 * it is after this core has left it, and reading it again would let
		 * another core's set slip in between. */
		if (0 == atomic_rmw_and_fetch(&tmr->timer_sharing->shared_cpu,
		             ~(0x1u << cpuid), __ATOMIC_RELAXED)) {
			atomic_rmw_and_fetch(&tmr->state, ~(uint32_t)SYS_TIMER_STATE_STARTED,
			    __ATOMIC_RELAXED);
		}
	}
	else {
		atomic_rmw_and_fetch(&tmr->state, ~(uint32_t)SYS_TIMER_STATE_STARTED,
		    __ATOMIC_RELAXED);
	}
	ipl_restore(ipl);
#else
	tmr->state &= ~SYS_TIMER_STATE_STARTED;
#endif
}

static inline bool sys_timer_is_periodic(struct sys_timer *tmr) {
	return tmr->flags & SYS_TIMER_PERIODIC;
}

static inline int sys_timer_is_inited(struct sys_timer *tmr) {
	return tmr->handle ? 1 : 0;
}

/** Type declaration for system timer structure */
typedef struct sys_timer sys_timer_t;

extern struct sys_timer *sys_timer_alloc(void);

extern void sys_timer_free(struct sys_timer *tmr);


/**
 * Make timer bare initialization.
 * Note, this not schedules timer.
 *
 * @param tmr
 * @param flags
 * @param handler
 * @param param
 */
extern int sys_timer_init(struct sys_timer *tmr, unsigned int flags,
    sys_timer_handler_t handler, void *param);

/**
 * Schedule timer to fire after specified amount of jiffes. Weither timer
 * will be rescheduled determed by @c flags specified on initialization
 *
 * @param tmr
 * @param jiffies
 */
extern void sys_timer_start(struct sys_timer *tmr, clock_t jiffies);

extern void sys_timer_stop(struct sys_timer *tmr);

/**
 * Set 'handle' timer for executing every 'ticks' ms.
 * Memory for set_tmr instance should be allocated before run timer_init_start.
 *
 * @param ptimer is pointer to preallocated buffer for system timer pointer.
 * @param ticks assignable time (quantity of milliseconds)
 * @param handler the function to be executed
 *
 * @return whether the timer is set
 * @retval 0 if the timer is set
 * @retval non-0 if the timer isn't set
 */
extern int sys_timer_init_start_msec(struct sys_timer *tmr, unsigned int flags,
    uint32_t ticks, sys_timer_handler_t handler, void *param);

/**
 * Set @c handle timer for executing every @c jiffies of hardware timer ticks.
 *
 * @see description of sys_timer_set().
 * @remarks
 *    This function should call @c handler NO LESS then after @c jiffies ticks.
 */
extern int sys_timer_init_start(struct sys_timer *tmr, unsigned int flags,
    clock_t jiffies, sys_timer_handler_t handler, void *param);

/**
 * Set 'handle' timer for executing every 'ticks' ms.
 * Memory for set_tmr instance will be allocated inside sys_timer_set.
 *
 * @param ptimer is pointer to buffer of sys_timer_t *.
 * @param ticks assignable time (quantity of milliseconds)
 * @param handler the function to be executed
 *
 * @return whether the timer is set
 * @retval 0 if the timer is set
 * @retval non-0 if the timer isn't set
 */
extern int sys_timer_set(struct sys_timer **ptimer, unsigned int flags,
    uint32_t ticks, sys_timer_handler_t handler, void *param);

/**
 * Shut down timer with system_tmr_t identity
 *
 * @param id timer identifier
 */
extern int sys_timer_close(struct sys_timer *ptimer);

__END_DECLS

#endif /* KERNEL_TIME_SYS_TIMER_H_ */
