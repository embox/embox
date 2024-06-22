/**
 * @file
 *
 * @brief Interface of periodical timers with milliseconds precision.
 *
 * @details
 *   for set timer use `timer_init_start' or `timer_set' functions.
 *   for emulate non-periodical behavior use timer_close function in the end of handler.
 *
 * @date 20.07.10
 * @author Fedor Burdun
 * @author Ilia Vaprol
 */

#ifndef KERNEL_TIME_TIMER_H_
#define KERNEL_TIME_TIMER_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/time/timer_strat.h>

__BEGIN_DECLS

struct sys_timer;

/**
 * Type of timer's handler. Function that will be called by timers every n-th
 * milliseconds.
 */
typedef void (*sys_timer_handler_t)(struct sys_timer *timer, void *param);

#define TIMER_PERIODIC       0x1
#define TIMER_ONESHOT        0x0
#define TIMER_REALTIME       0x80000000

#define TIMER_STATE_PREALLOC 0x1
#define TIMER_STATE_STARTED  0x2

/**
 * system timer structure
 */
struct sys_timer {
	sys_timer_queue_t lnk;
	struct dlist_head st_wait_link;

	uint32_t load;
	/* Clocks count at which timer should fire: cnt = current clocks() + load. */
	clock_t cnt;
	sys_timer_handler_t handle;
	void *param;
	unsigned int flags;
	uint32_t state; /**< do we use timer_set or timer_init_start? */
};

static inline bool timer_is_preallocated(struct sys_timer *tmr) {
	return tmr->state & TIMER_STATE_PREALLOC;
}

static inline void timer_set_preallocated(struct sys_timer *tmr) {
	tmr->state |= TIMER_STATE_PREALLOC;
}

static inline void timer_clear_preallocated(struct sys_timer *tmr) {
	tmr->state &= ~TIMER_STATE_PREALLOC;
}

static inline bool timer_is_started(struct sys_timer *tmr) {
	return tmr->state & TIMER_STATE_STARTED;
}

static inline void timer_set_started(struct sys_timer *tmr) {
	tmr->state |= TIMER_STATE_STARTED;
}

static inline void timer_set_stopped(struct sys_timer *tmr) {
	tmr->state &= ~TIMER_STATE_STARTED;
}

static inline bool timer_is_periodic(struct sys_timer *tmr) {
	return tmr->flags & TIMER_PERIODIC;
}

static inline int timer_is_inited(struct sys_timer *tmr) {
	return tmr->handle ? 1 : 0;
}

/** Type declaration for system timer structure */
typedef struct sys_timer sys_timer_t;

/**
 * Make timer bare initialization.
 * Note, this not schedules timer.
 *
 * @param tmr
 * @param flags
 * @param handler
 * @param param
 */
extern int timer_init(struct sys_timer *tmr, unsigned int flags,
    sys_timer_handler_t handler, void *param);

/**
 * Schedule timer to fire after specified amount of jiffes. Weither timer
 * will be rescheduled determed by @c flags specified on initialization
 *
 * @param tmr
 * @param jiffies
 */
extern void timer_start(struct sys_timer *tmr, clock_t jiffies);

extern void timer_stop(struct sys_timer *tmr);

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
extern int timer_init_start_msec(struct sys_timer *tmr, unsigned int flags,
    uint32_t ticks, sys_timer_handler_t handler, void *param);

/**
 * Set @c handle timer for executing every @c jiffies of hardware timer ticks.
 *
 * @see description of timer_set().
 * @remarks
 *    This function should call @c handler NO LESS then after @c jiffies ticks.
 */
extern int timer_init_start(struct sys_timer *tmr, unsigned int flags,
    clock_t jiffies, sys_timer_handler_t handler, void *param);

/**
 * Set 'handle' timer for executing every 'ticks' ms.
 * Memory for set_tmr instance will be allocated inside timer_set.
 *
 * @param ptimer is pointer to buffer of sys_timer_t *.
 * @param ticks assignable time (quantity of milliseconds)
 * @param handler the function to be executed
 *
 * @return whether the timer is set
 * @retval 0 if the timer is set
 * @retval non-0 if the timer isn't set
 */
extern int timer_set(struct sys_timer **ptimer, unsigned int flags,
    uint32_t ticks, sys_timer_handler_t handler, void *param);

/**
 * Shut down timer with system_tmr_t identity
 *
 * @param id timer identifier
 */
extern int timer_close(struct sys_timer *ptimer);

__END_DECLS

#endif /* KERNEL_TIME_TIMER_H_ */
