/**
 * @file
 * @brief Multi-threads Timer handling.
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */

#include <mem/misc/pool.h>
#include <hal/clock.h>
#include <lib/list.h>
#include <embox/unit.h>
#include <kernel/timer.h>
#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>
#include <kernel/clock_source.h>
#include <errno.h>

#define TIMER_POOL_SZ 20 /**<system timers quantity */

EMBOX_UNIT_INIT(timer_init);

clock_t sys_ticks; /* ticks after start system. used from time.c */

POOL_DEF(timer_pool, sys_tmr_t, TIMER_POOL_SZ);
static struct list_head *sys_timers_list;

int init_timer(sys_tmr_t *ptimer, uint32_t ticks,
		TIMER_FUNC handler, void *param) {
	if (!handler || !ptimer) {
		return -EINVAL;
	}

	ptimer->is_preallocated = false;
	ptimer->cnt    = ptimer->load = ticks;
	ptimer->handle = handler;
	ptimer->param  = param;
	list_add_tail(&ptimer->lnk, sys_timers_list);

	return 0;
}

int set_timer(sys_tmr_t **ptimer, uint32_t ticks,
		TIMER_FUNC handler, void *param) {

	if (NULL == handler || NULL == ptimer) {
		return -EINVAL;
	}
	if (NULL == (*ptimer = (sys_tmr_t*) pool_alloc(&timer_pool))) {
		return -ENOMEM;
	}
	/* we know that init will be success (right ptimer and handler) */
	init_timer(*ptimer, ticks, handler, param);
	(*ptimer)->is_preallocated = true;

	return 0;
}

int close_timer(sys_tmr_t *ptimer) {
	if (NULL == ptimer) {
		return -EINVAL;
	}

	list_del((struct list_head *) ptimer);
	if (ptimer->is_preallocated) {
		pool_free(&timer_pool, ptimer);
	}

	return 0;
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
static void inc_sys_timers(void) {
	struct list_head *tmp, *tmp2;
	sys_tmr_t *tmr;

	list_for_each_safe(tmp, tmp2, sys_timers_list) {
		tmr = (sys_tmr_t*) tmp;
		if (0 == tmr->cnt--) {
			tmr->handle(tmr, tmr->param);
			tmr->cnt = tmr->load;
		}
	}
}

/**
 * Handling of the clock tick.
 */
void clock_tick_handler(int irq_num, void *dev_id) {
	sys_ticks++;
	inc_sys_timers();
}

/**
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
int timer_init(void) {
	sys_ticks = 0;
	clock_init();
	clock_setup(clock_source_get_precision());
	sys_timers_list = clock_source_get_timers_list();
	return 0;
}
