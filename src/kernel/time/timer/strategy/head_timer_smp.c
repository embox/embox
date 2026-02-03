/**
 * @file
 * @brief
 *
 * @date 22.07.10
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 * @author Fedor Burdun
 * @date 26.08.2024
 * @author Zeng Zixian
 */

#include <kernel/time/sys_timer.h>
#include <kernel/sched.h>
#include <hal/clock.h>
#include <hal/ipl.h>
#include <hal/cpu.h>

#include <embox/unit.h>

/* init at run time */
static struct dlist_head sys_timers_list[NCPU];

EMBOX_UNIT_INIT(sys_timers_list_init);

static int sys_timers_list_init(void) {
	for(int i = 0; i < NCPU; i++)
		dlist_head_init(&sys_timers_list[i]);
	return 0;
}

void timer_strat_start(struct sys_timer *tmr) {
	int cpuid;
	struct sys_timer *it_tmr = NULL;
	struct dlist_head *next_tmr_lnk;
	ipl_t ipl;

	tmr->cnt = clock_sys_ticks() + tmr->load;

	ipl = ipl_save();
	cpuid = cpu_get_id();
	ipl_restore(ipl);

	if(sched_ticker_get_timer() == (void*)tmr) {
		dlist_head_init(&tmr->multi_lnk[cpuid]);
	}else{
		dlist_head_init(&tmr->lnk);
	}
	next_tmr_lnk = &sys_timers_list[cpuid];

	for (struct dlist_head *__next = sys_timers_list[cpuid].next; \
		__next != &sys_timers_list[cpuid]; __next = __next->next) {

		it_tmr =  dlist_entry(__next, struct sys_timer, multi_lnk[cpuid]);
		if(sched_ticker_get_timer() != (void*)it_tmr) {
			it_tmr =  dlist_entry(__next, struct sys_timer, lnk);
		}

		if (it_tmr->cnt >= tmr->cnt) {
			/* decrease value of next timer after inserting */
			it_tmr->cnt -= tmr->cnt;

			if(sched_ticker_get_timer() == (void*)it_tmr) {
				next_tmr_lnk = &it_tmr->multi_lnk[cpuid];
			}else{
				next_tmr_lnk = &it_tmr->lnk;
			}
			break;
		}
		tmr->cnt -= it_tmr->cnt;
	}

	sys_timer_set_started(tmr);
	if(sched_ticker_get_timer() == (void*)tmr) {
		dlist_add_prev(&tmr->multi_lnk[cpuid], next_tmr_lnk);
	}else{
		dlist_add_prev(&tmr->lnk, next_tmr_lnk);
	}
}

void timer_strat_stop(struct sys_timer *tmr) {
	int cpuid;
	struct sys_timer *next_tmr;
	ipl_t ipl;

	ipl = ipl_save();
	cpuid = cpu_get_id();
	ipl_restore(ipl);

	sys_timer_set_stopped(tmr);

	if(sched_ticker_get_timer() == (void*)tmr) {
		if (tmr->multi_lnk[cpuid].next != &sys_timers_list[cpuid]) {
			next_tmr = dlist_entry(tmr->multi_lnk[cpuid].next, struct sys_timer, lnk);
			next_tmr->cnt += tmr->cnt;
		}
	}else{
		if (tmr->lnk.next != &sys_timers_list[cpuid]) {
			next_tmr = dlist_entry(tmr->lnk.next, struct sys_timer, lnk);
			next_tmr->cnt += tmr->cnt;
		}
	}


	if(sched_ticker_get_timer() == (void*)tmr) {
		assert(tmr != next_tmr); /* At most one sched_ticker in list */
		dlist_del(&tmr->multi_lnk[cpuid]);
	}else{
		dlist_del(&tmr->lnk);
	}

}

int timer_strat_get_next_event(clock_t *next_event) {
	int cpuid;
	ipl_t ipl;
	int ret = -1;
	struct sys_timer *tmr;

	ipl = ipl_save();
	cpuid = cpu_get_id();
	ipl_restore(ipl);

	if (!dlist_empty(&sys_timers_list[cpuid])) {
		/* have a try to check whether the next timer is sched_ticker */
		tmr = dlist_first_entry(&sys_timers_list[cpuid], struct sys_timer, multi_lnk[cpuid]);
		if(sched_ticker_get_timer() != (void*)tmr) {
			tmr = dlist_first_entry(&sys_timers_list[cpuid], struct sys_timer, lnk);
		}
		*next_event = tmr->cnt;
		ret = 0;
	}

	return ret;
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
void timer_strat_sched(clock_t jiffies) {
	int cpuid;
	struct sys_timer *it_tmr = NULL;
	ipl_t ipl;

	ipl = ipl_save();
	cpuid = cpu_get_id();
	ipl_restore(ipl);

	for (struct dlist_head *__next = sys_timers_list[cpuid].next; \
		__next != &sys_timers_list[cpuid]; __next = __next->next) {

		it_tmr =  dlist_entry(__next, struct sys_timer, multi_lnk[cpuid]);
		if(sched_ticker_get_timer() != (void*)it_tmr) {
			it_tmr =  dlist_entry(__next, struct sys_timer, lnk);
		}

		if (jiffies < it_tmr->cnt) {
			break;
		}
		jiffies -= it_tmr->cnt;

		timer_strat_stop(it_tmr);
		if (sys_timer_is_periodic(it_tmr)) {
			timer_strat_start(it_tmr);
		}

		it_tmr->handle(it_tmr, it_tmr->param);
	}

}
