/**
 * @file
 * @brief Multi-threads Timer handling.
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */

#include <lib/libds/dlist.h>

#include <hal/ipl.h>
#include <hal/clock.h>

#include <kernel/time/timer.h>

static DLIST_DEFINE(sys_timers_list);

void timer_strat_start(struct sys_timer *tmr) {
	ipl_t ipl;

	timer_set_started(tmr);

	dlist_head_init(&tmr->lnk);

	ipl = ipl_save();
	/* new timer added to begining of the list to sure, that it will not
 	 * be processed. Otherwise, if a new timer added while handling some
	 * other one, it will be processed during same timer_strat_sched
	 * request, not the next. */
	dlist_add_next(&tmr->lnk, &sys_timers_list);
	ipl_restore(ipl);
}

int timer_strat_get_next_event(clock_t *next_event) {
	struct sys_timer *tmr;
	clock_t next = (clock_t) -1;
	ipl_t ipl;

	ipl = ipl_save();

	dlist_foreach_entry(tmr, &sys_timers_list, lnk) {
		if (tmr->cnt < next) {
			next = tmr->cnt;
		}
	}

	ipl_restore(ipl);

	if (next == (clock_t) -1) {
		return -1;
	}

	*next_event = next;

	return 0;
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
void timer_strat_sched(clock_t jiffies) {
	sys_timer_t *tmr;

	dlist_foreach_entry(tmr, &sys_timers_list, lnk) {
		if (jiffies >= tmr->cnt) {
			if (timer_is_periodic(tmr)) {
				tmr->cnt = clock_sys_ticks() + tmr->load;
			} else {
				timer_strat_stop(tmr);
			}

			tmr->handle(tmr, tmr->param);
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
