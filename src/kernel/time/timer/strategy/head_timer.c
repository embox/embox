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

static DLIST_DEFINE(sys_timers_list); /* list head to timers */

void timer_strat_start(struct sys_timer *tmr) {
	struct sys_timer *it_tmr;

	dlist_head_init(&tmr->lnk);
	timer_set_started(tmr);

	tmr->cnt = clock_sys_ticks() + tmr->load;

	/* find first element that its time bigger than inserting @new_time */
	dlist_foreach_entry(it_tmr, &sys_timers_list, lnk) {
		if (it_tmr->cnt >= tmr->cnt) {
			/* decrease value of next timer after inserting */
			it_tmr->cnt -= tmr->cnt;

			dlist_add_prev(&tmr->lnk, &it_tmr->lnk);

			return;
		}
		tmr->cnt -= it_tmr->cnt;

	}

	/* add the latest timer to end of list */
	dlist_add_prev(&tmr->lnk, &sys_timers_list);
}

void timer_strat_stop(struct sys_timer *ptimer) {
	struct sys_timer *next_tmr;

	timer_set_stopped(ptimer);

	if (ptimer->lnk.next != &sys_timers_list) {
		next_tmr = (struct sys_timer *)ptimer->lnk.next;
		next_tmr->cnt += ptimer->cnt;
	}

	dlist_del(&ptimer->lnk);
}

bool timer_strat_need_sched(clock_t jiffies) {
	struct sys_timer *t;

	if (dlist_empty(&sys_timers_list)) {
		return false;
	}
	t = dlist_first_entry(&sys_timers_list, struct sys_timer, lnk);
	return jiffies >= t->cnt;
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
void timer_strat_sched(clock_t jiffies) {
	struct sys_timer *timer;

	dlist_foreach_entry(timer, &sys_timers_list, lnk) {
		if (jiffies < timer->cnt) {
			break;
		}
		jiffies -= timer->cnt;

		timer_strat_stop(timer);
		if (timer_is_periodic(timer)) {
			timer_strat_start(timer);
		}

		timer->handle(timer, timer->param);
	}
}
