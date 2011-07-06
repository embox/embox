/**
 * @file
 * @brief Multi-threads Timer handling.
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */

#include <util/pool.h>
#include <hal/clock.h>
#include <lib/list.h>
//#include <util/list.h>
#include <embox/unit.h>
#include <kernel/timer.h>
#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>

EMBOX_UNIT_INIT(timer_init);

struct sys_tmr {
	struct list_head *next, *prev;
	uint32_t   load;
	uint32_t   cnt;
	TIMER_FUNC handle;
	void       *param;
//	uint32_t   per; // is periodical timer
};

static volatile clock_t cnt_sys_time; /**< quantity ms after start system */

POOL_DEF(timer_pool, sys_tmr_t, TIMER_POOL_SZ);
static LIST_HEAD(sys_timers_list);

clock_t cnt_system_time(void) {
	return cnt_sys_time;
}

int set_timer(sys_tmr_ptr *ptimer, uint32_t ticks,
		TIMER_FUNC handle, void *param) {
	sys_tmr_ptr new_timer;

	if (!handle || !(new_timer = (sys_tmr_ptr) pool_alloc(&timer_pool))) {
		if (ptimer) { // it's necessary?
			*ptimer = NULL;
		}
		return 1;
	}
	new_timer->cnt    = new_timer->load = ticks;
	new_timer->handle = handle;
	new_timer->param  = param;
	list_add_tail((struct list_head *) new_timer, &sys_timers_list);
	if (ptimer) {
		*ptimer = new_timer;
	}
	return 0;
}

int close_timer(sys_tmr_ptr *ptimer) {
	if (ptimer) {
		list_del((struct list_head *) *ptimer);
		pool_free(&timer_pool, *ptimer);
		*ptimer = NULL;
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
	sys_tmr_ptr tmr;

	list_for_each_safe(tmp, tmp2, &sys_timers_list) {
		tmr = (sys_tmr_ptr) tmp;
		if (0 == tmr->cnt--) {
			if (tmr->handle) {
				tmr->handle(tmr, tmr->param);
			}
#if 0
			if (!tmr->per) { // if non-periodical => delete it
				close_timer(tmr);
			}
#endif
			tmr->cnt = tmr->load;
		}
	}
}

/**
 * Handling of the clock tick.
 */
void clock_tick_handler(int irq_num, void *dev_id) {
	cnt_sys_time++;
	inc_sys_timers();
}

/**
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
int timer_init(void) {
	cnt_sys_time = 0;
	clock_init();
	clock_setup(TIMER_FREQUENCY);
	return 0;
}
