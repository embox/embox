/**
 * @file
 * @brief
 *
 * @date 22.07.10
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 * @author Fedor Burdun
 */

#include <kernel/time/timer.h>
#include <hal/clock.h>
#include <hal/ipl.h>

static DLIST_DEFINE(sys_timers_list); /* list head to timers */

void timer_strat_start(struct sys_timer *tmr) {
	struct sys_timer *it_tmr = NULL;
	struct dlist_head *next_tmr_lnk = &sys_timers_list;
	ipl_t ipl;

	dlist_head_init(&tmr->lnk);

	tmr->cnt = clock_sys_ticks() + tmr->load;

	ipl = ipl_save();

	/* find first element that its time bigger than inserting @new_time */
	dlist_foreach_entry(it_tmr, &sys_timers_list, lnk) {
		if (it_tmr->cnt >= tmr->cnt) {
			/* decrease value of next timer after inserting */
			it_tmr->cnt -= tmr->cnt;

			next_tmr_lnk = &it_tmr->lnk;
			goto out;
		}
		tmr->cnt -= it_tmr->cnt;
	}

out:

	sys_timer_set_started(tmr);
	dlist_add_prev(&tmr->lnk, next_tmr_lnk);
	ipl_restore(ipl);
}

void timer_strat_stop(struct sys_timer *ptimer) {
	struct sys_timer *next_tmr;
	ipl_t ipl;

	ipl = ipl_save();
	sys_timer_set_stopped(ptimer);

	if (ptimer->lnk.next != &sys_timers_list) {
		next_tmr = (struct sys_timer *)ptimer->lnk.next;
		next_tmr->cnt += ptimer->cnt;
	}

	dlist_del(&ptimer->lnk);
	ipl_restore(ipl);
}

int timer_strat_get_next_event(clock_t *next_event) {
	ipl_t ipl;
	int ret = -1;
	struct sys_timer *t;

	ipl = ipl_save();
	if (!dlist_empty(&sys_timers_list)) {
		t = dlist_first_entry(&sys_timers_list, struct sys_timer, lnk);
		*next_event = t->cnt;
		ret = 0;
	}
	ipl_restore(ipl);

	return ret;
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
void timer_strat_sched(clock_t jiffies) {
	struct sys_timer *timer = NULL;
	ipl_t ipl;

	ipl = ipl_save();
	dlist_foreach_entry(timer, &sys_timers_list, lnk) {
		if (jiffies < timer->cnt) {
			break;
		}
		jiffies -= timer->cnt;

		timer_strat_stop(timer);
		if (sys_timer_is_periodic(timer)) {
			timer_strat_start(timer);
		}

		timer->handle(timer, timer->param);
	}
	ipl_restore(ipl);
}
