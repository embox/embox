/**
 * @file
 * @brief Multi-threads Timer handling.
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */

#include <kernel/time/timer.h>
#include <profiler/tracing/trace.h>
#include <util/dlist.h>

#include <hal/ipl.h>

static DLIST_DEFINE(sys_timers_list);

void timer_strat_start(struct sys_timer *tmr) {
	ipl_t ipl;

	timer_set_started(tmr);

	dlist_head_init(&tmr->lnk);

	ipl = ipl_save();
	dlist_add_prev(&tmr->lnk, &sys_timers_list);
	ipl_restore(ipl);
}
/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
void timer_strat_sched(void) {
	struct dlist_head *tmp, *tmp2;
	sys_timer_t *tmr;

	dlist_foreach(tmp, tmp2, &sys_timers_list) {
		tmr = (sys_timer_t*) tmp;
		if (0 == tmr->cnt--) {
			trace_point("timer tick");
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
	ipl_t ipl;

	timer_set_stopped(tmr);

	ipl = ipl_save();
	dlist_del(&tmr->lnk);
	ipl_restore(ipl);
}
