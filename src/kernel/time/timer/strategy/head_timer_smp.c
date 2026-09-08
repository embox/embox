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

#include <assert.h>

#include <kernel/time/sys_timer.h>
#include <kernel/sched.h>
#include <kernel/spinlock.h>
#include <hal/clock.h>
#include <hal/ipl.h>
#include <hal/cpu.h>

#include <embox/unit.h>

/* init at run time */
static struct dlist_head sys_timers_list[NCPU];

/* One lock per list. ipl_save() is enough for the three entry points that
 * only touch their own core's list; it is not enough for timer_strat_stop(),
 * which mends the list of tmr->owner_cpu and may be running on any other
 * core. */
static spinlock_t sys_timers_lock[NCPU];

EMBOX_UNIT_INIT(sys_timers_list_init);

static int sys_timers_list_init(void) {
	for(int i = 0; i < NCPU; i++) {
		dlist_head_init(&sys_timers_list[i]);
		spin_init(&sys_timers_lock[i], __SPIN_UNLOCKED);
	}
	return 0;
}

/* The sched ticker is on every core's list at once and
 * is linked by multi_lnk[cpuid]; every other timer is linked by lnk. So the
 * timer a list node belongs to cannot be recovered without asking which. Both
 * candidates are address arithmetic -- nothing is read through them -- and the
 * comparison against the ticker is what decides. */
/* The counter of a timer as queued in `cpuid`'s list. The ticker is on every
 * list and keeps one per list; everything else is on exactly one and keeps the
 * single `cnt`. An lvalue, because every user either reads it or does delta
 * arithmetic on it in place.
 *
 * `ticker` is passed in rather than fetched here: sched_ticker_get_timer() is
 * an out-of-line call at -O0, this macro appears two to four times per list
 * entry visited, and the answer cannot change while a list is being walked. */
#define TIMER_CNT(tmr, cpuid, ticker)                     \
	(*(((void *)(tmr) == (ticker)) ? &(tmr)->multi_cnt[cpuid] : &(tmr)->cnt))

static inline struct sys_timer *timer_from_lnk(struct dlist_head *lnk,
    int cpuid) {
	struct sys_timer *tmr;

	tmr = dlist_entry(lnk, struct sys_timer, multi_lnk[cpuid]);
	if (sched_ticker_get_timer() != (void *)tmr) {
		tmr = dlist_entry(lnk, struct sys_timer, lnk);
	}

	return tmr;
}

/* The list edit itself. Called with interrupts masked and
 * sys_timers_lock[cpuid] held; `cpuid` names the list, and is not necessarily
 * this core. */
static void __timer_strat_start(struct sys_timer *tmr, int cpuid) {
	struct sys_timer *it_tmr = NULL;
	struct dlist_head *next_tmr_lnk;

	const void *__ticker = sched_ticker_get_timer();

	TIMER_CNT(tmr, cpuid, __ticker) = clock_sys_ticks() + tmr->load;

	if(sched_ticker_get_timer() == (void*)tmr) {
		dlist_head_init(&tmr->multi_lnk[cpuid]);
	}else{
		dlist_head_init(&tmr->lnk);
	}
	next_tmr_lnk = &sys_timers_list[cpuid];

	for (struct dlist_head *__next = sys_timers_list[cpuid].next; \
		__next != &sys_timers_list[cpuid]; __next = __next->next) {

		it_tmr = timer_from_lnk(__next, cpuid);

		if (TIMER_CNT(it_tmr, cpuid, __ticker) >= TIMER_CNT(tmr, cpuid, __ticker)) {
			/* decrease value of next timer after inserting */
			TIMER_CNT(it_tmr, cpuid, __ticker) -= TIMER_CNT(tmr, cpuid, __ticker);

			if(sched_ticker_get_timer() == (void*)it_tmr) {
				next_tmr_lnk = &it_tmr->multi_lnk[cpuid];
			}else{
				next_tmr_lnk = &it_tmr->lnk;
			}
			break;
		}
		TIMER_CNT(tmr, cpuid, __ticker) -= TIMER_CNT(it_tmr, cpuid, __ticker);
	}

	sys_timer_set_started(tmr);
	if(sched_ticker_get_timer() == (void*)tmr) {
		dlist_add_prev(&tmr->multi_lnk[cpuid], next_tmr_lnk);
	}else{
		/* From here until it is stopped, this timer belongs to this CPU's list
		 * -- wherever its owner ends up running. */
		tmr->owner_cpu = cpuid;
		dlist_add_prev(&tmr->lnk, next_tmr_lnk);
	}
}

void timer_strat_start(struct sys_timer *tmr) {
	int cpuid;
	ipl_t ipl;

	/* Interrupts stay off for the whole edit, the way head_timer does it.
	 * This list belongs to one CPU, but the timer interrupt of that CPU reads
	 * it too (clock_handle_ticks -> timer_strat_get_next_event), and a
	 * half-linked list read from there is a wrong answer about when the next
	 * timer is due.
	 *
	 * So: interrupts AND the lock, because another core stopping a timer it
	 * owns here edits the same deltas. Interrupts first: cpu_get_id() reads
	 * per-CPU state and must not be preempted between the read and its use. */
	ipl = ipl_save();
	cpuid = cpu_get_id();
	__spin_lock(&sys_timers_lock[cpuid]);

	__timer_strat_start(tmr, cpuid);

	__spin_unlock(&sys_timers_lock[cpuid]);
	ipl_restore(ipl);
}

/* The list edit itself, as for __timer_strat_start. */
static void __timer_strat_stop(struct sys_timer *tmr, int cpuid) {
	/* NULL, because the assert below reads it on
	 * the path where the `if` that assigns it did not run. */
	struct sys_timer *next_tmr = NULL;
	const void *__ticker = sched_ticker_get_timer();

	sys_timer_set_stopped(tmr);

	if(sched_ticker_get_timer() == (void*)tmr) {
		if (tmr->multi_lnk[cpuid].next != &sys_timers_list[cpuid]) {
			next_tmr = timer_from_lnk(tmr->multi_lnk[cpuid].next, cpuid);
			TIMER_CNT(next_tmr, cpuid, __ticker) += TIMER_CNT(tmr, cpuid, __ticker);
		}
	}else{
		if (tmr->lnk.next != &sys_timers_list[cpuid]) {
			/* This one used to assume lnk. When the successor was the ticker
			 * it added tmr->cnt to a static 16 * (1 + cpuid) bytes past it. */
			next_tmr = timer_from_lnk(tmr->lnk.next, cpuid);
			TIMER_CNT(next_tmr, cpuid, __ticker) += TIMER_CNT(tmr, cpuid, __ticker);
		}
	}


	if(sched_ticker_get_timer() == (void*)tmr) {
		assert(tmr != next_tmr); /* At most one sched_ticker in list */
		dlist_del(&tmr->multi_lnk[cpuid]);
	}else{
		dlist_del(&tmr->lnk);
		/* Off the list, so the next stop without a start in between trips the
		 * assert in timer_strat_stop() rather than mending a list this timer
		 * is no longer on. */
		tmr->owner_cpu = -1;
	}
}

void timer_strat_stop(struct sys_timer *tmr) {
	int cpuid;
	ipl_t ipl;

	ipl = ipl_save();
	if (sched_ticker_get_timer() == (void*)tmr) {
		/* On every list at once, and this is the copy we are unlinking. */
		cpuid = cpu_get_id();
	}
	else {
		/* The list to mend is the one the timer was started on. A timed wait
		 * starts its timer before schedule() and closes it after, and those
		 * two can be different CPUs; asking cpu_get_id() here would compare
		 * tmr->lnk.next against the wrong list head and add tmr->cnt to
		 * whatever static follows sys_timers_list in .bss.
		 *
		 * And this is exactly why the list needs a lock: masking interrupts
		 * stops this core; the core that owns the list keeps running. */
		cpuid = tmr->owner_cpu;
		assertf(cpuid >= 0 && cpuid < NCPU,
		    "timer %p stopped while queued on cpu %d", tmr, cpuid);
	}
	__spin_lock(&sys_timers_lock[cpuid]);

	__timer_strat_stop(tmr, cpuid);

	__spin_unlock(&sys_timers_lock[cpuid]);
	ipl_restore(ipl);
}

int timer_strat_get_next_event(clock_t *next_event) {
	int cpuid;
	ipl_t ipl;
	int ret = -1;
	struct sys_timer *tmr;
	const void *__ticker = sched_ticker_get_timer();

	ipl = ipl_save();
	cpuid = cpu_get_id();
	__spin_lock(&sys_timers_lock[cpuid]);

	if (!dlist_empty(&sys_timers_list[cpuid])) {
		/* have a try to check whether the next timer is sched_ticker */
		tmr = timer_from_lnk(sys_timers_list[cpuid].next, cpuid);
		*next_event = TIMER_CNT(tmr, cpuid, __ticker);
		ret = 0;
	}

	__spin_unlock(&sys_timers_lock[cpuid]);
	ipl_restore(ipl);

	return ret;
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
void timer_strat_sched(clock_t jiffies) {
	int cpuid;
	ipl_t ipl;
	const void *__ticker = sched_ticker_get_timer();

	ipl = ipl_save();
	cpuid = cpu_get_id();

	/* Head at a time, and the handler runs with the lock dropped.
	 *
	 * Head at a time because unlinking the head folds its absolute expiry into
	 * its successor's delta -- the new head is absolute again -- so one
	 * comparison per turn is exact.
	 *
	 * Lock dropped because handlers arm and disarm timers: sched_ticker
	 * re-arms itself, and a timed wait's handler wakes a thread, which is a
	 * trip through the scheduler. Holding a spinlock across that would
	 * deadlock against this core's own next timer edit. */
	while (1) {
		struct sys_timer *tmr = NULL;

		__spin_lock(&sys_timers_lock[cpuid]);
		if (!dlist_empty(&sys_timers_list[cpuid])) {
			struct sys_timer *head;

			head = timer_from_lnk(sys_timers_list[cpuid].next, cpuid);
			if (jiffies >= TIMER_CNT(head, cpuid, __ticker)) {
				tmr = head;
				__timer_strat_stop(tmr, cpuid);
				if (sys_timer_is_periodic(tmr)) {
					__timer_strat_start(tmr, cpuid);
				}
			}
		}
		__spin_unlock(&sys_timers_lock[cpuid]);

		if (!tmr) {
			break;
		}

		tmr->handle(tmr, tmr->param);
	}
	ipl_restore(ipl);
}
