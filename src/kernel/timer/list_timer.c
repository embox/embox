/**
 * @file
 * @brief Multi-threads Timer handling.
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */
#include <lib/list.h>
#include <kernel/timer.h>

static LIST_HEAD(sys_timers_list);

void timer_strat_start(struct sys_timer *tmr) {
	timer_set_started(tmr);
	list_add_tail(&tmr->lnk, &sys_timers_list);
}
/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
void timer_strat_sched(void) {
	struct list_head *tmp, *tmp2;
	sys_timer_t *tmr;

	list_for_each_safe(tmp, tmp2, &sys_timers_list) {
		tmr = (sys_timer_t*) tmp;
		if (0 == tmr->cnt--) {
			tmr->handle(tmr, tmr->param);
			if(!timer_is_periodic(tmr)) {
				timer_strat_stop(tmr);
				continue;
			}
			tmr->cnt = tmr->load;
		}
	}
}

void timer_strat_stop(struct sys_timer *tmr) {
	timer_set_stopped(tmr);
	list_del(&tmr->lnk);
}
