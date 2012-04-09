/**
 * @file
 * @brief
 *
 * @date 22.07.10
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 * @author Fedor Burdun
 */

#include <mem/misc/pool.h>
#include <lib/list.h>
#include <kernel/timer.h>


static LIST_HEAD(sys_timers_list); /* list head to timers */

void timer_start(struct sys_timer *tmr) {
	struct list_head *iter, *tmp;

	tmr->cnt = tmr->load;

	/* if we haven't any timers we just insert new timer into the list */
	if (list_empty(&sys_timers_list)) {
		/* we just add timer to list */
		list_add(&tmr->lnk, &sys_timers_list);
		return;
	}

	/* find first element that its time bigger than inserting @new_time */
	list_for_each_safe(iter, tmp, &sys_timers_list) {
		struct sys_timer *it_tmr = (struct sys_timer*) iter;

		if (it_tmr->cnt >= tmr->cnt) {
			/* decrease value of next timer after inserting */
			it_tmr->cnt -= tmr->cnt;
			list_add_tail(&tmr->lnk, iter);
			return;
		}
		tmr->cnt -= it_tmr->cnt;

	}

	/* add the latest timer to end of list */
	list_add_tail(&tmr->lnk, &sys_timers_list);
}

void timer_stop(struct sys_timer *ptimer) {
	list_del(&ptimer->lnk);
}

static inline bool timers_need_schedule(void) {
	if(list_empty(&sys_timers_list)) {
		return false;
	}

	if(0 == ((sys_timer_t*)sys_timers_list.next)->cnt) {
		return true;
	} else {
		((sys_timer_t*)sys_timers_list.next)->cnt--;
	}

	return false;
}

static inline void timers_schedule(void) {
	struct sys_timer *timer;
	struct list_head *iter, *tmp;

	list_for_each_safe(iter, tmp, &sys_timers_list) {
		timer = (struct sys_timer *)iter;

		if (0 != timer->cnt) {
			return;
		}

		timer->handle(timer, timer->param);

		list_del(iter);

		timer_start(timer);
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
