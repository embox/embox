/**
 * @file
 * @brief
 *
 * @date 22.07.10
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 * @author Fedor Burdun
 */

#include <lib/list.h>
#include <kernel/timer.h>


static LIST_HEAD(sys_timers_list); /* list head to timers */

void timer_start(struct sys_timer *tmr) {
	struct sys_timer *it_tmr, *tmp;
	//TODO patch for sleep(0)
	if(0 == tmr->load) {
		tmr->load = 1;
	}

	tmr->cnt = tmr->load;

	/* find first element that its time bigger than inserting @new_time */
	list_for_each_entry_safe(it_tmr, tmp, &sys_timers_list,lnk) {
		if (it_tmr->cnt >= tmr->cnt) {
			/* decrease value of next timer after inserting */
			it_tmr->cnt -= tmr->cnt;

			list_add_tail(&tmr->lnk, &it_tmr->lnk);
			return;
		}
		tmr->cnt -= it_tmr->cnt;

	}

	/* add the latest timer to end of list */
	list_add_tail(&tmr->lnk, &sys_timers_list);
}

void timer_stop(struct sys_timer *ptimer) {
	struct sys_timer *next_tmr;
	if(ptimer->lnk.next != &sys_timers_list) {
		next_tmr = (struct sys_timer *)ptimer->lnk.next;
		next_tmr->cnt += ptimer->cnt;
	}
	list_del(&ptimer->lnk);
}

static inline bool timers_need_schedule(void) {
	if(list_empty(&sys_timers_list)) {
		return false;
	}

	if(0 == --((sys_timer_t*)sys_timers_list.next)->cnt) {
		return true;
	} else {
		return false;
	}
}

static inline void timers_schedule(void) {
	struct sys_timer *timer,*tmp;
	uint32_t nxt_cnt = 0; /* we schedule first timer */

	list_for_each_entry_safe(timer, tmp, &sys_timers_list, lnk) {
		nxt_cnt = tmp->cnt;
		timer->handle(timer, timer->param);

		timer_stop(timer);
		timer_start(timer);

		if (0 != nxt_cnt) {
			return;
		}
	}
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
void timer_sched(void) {
	if(timers_need_schedule()) {
		timers_schedule();
	}
}
