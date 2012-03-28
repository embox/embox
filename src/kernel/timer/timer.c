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
//#include <kernel/thread/event.h>
//#include <kernel/thread/sched.h>
#include <kernel/clock_source.h>
#include <errno.h>

#define TIMER_POOL_SZ 20 /**< system timers quantity */

EMBOX_UNIT_INIT(module_init);

static clock_t sys_ticks; /* ticks after start system. */

clock_t clock(void) {
	return sys_ticks;
}

POOL_DEF(timer_pool, sys_timer_t, TIMER_POOL_SZ);
static struct list_head sys_timers_list;

int timer_init(sys_timer_t *ptimer, uint32_t ticks,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !ptimer) {
		return -EINVAL;
	}

	ptimer->is_preallocated = false;
	ptimer->cnt    = ptimer->load = ticks;
	ptimer->handle = handler;
	ptimer->param  = param;
	list_add_tail(&ptimer->lnk, &sys_timers_list);

	return 0;
}

int timer_set(sys_timer_t **ptimer, uint32_t ticks,
		sys_timer_handler_t handler, void *param) {

	if (NULL == handler || NULL == ptimer) {
		return -EINVAL;
	}
	if (NULL == (*ptimer = (sys_timer_t*) pool_alloc(&timer_pool))) {
		return -ENOMEM;
	}
	/* we know that init will be success (right ptimer and handler) */
	timer_init(*ptimer, ticks, handler, param);
	(*ptimer)->is_preallocated = true;

	return 0;
}

int timer_close(sys_timer_t *ptimer) {
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
	sys_timer_t *tmr;

	list_for_each_safe(tmp, tmp2, &sys_timers_list) {
		tmr = (sys_timer_t*) tmp;
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
int module_init(void) {
	sys_ticks = 0;
	INIT_LIST_HEAD(&sys_timers_list);
	clock_init();
	clock_setup(clock_source_get_precision(NULL));
//	sys_timers_list = clock_source_get_timers_list();
	return 0;
}
