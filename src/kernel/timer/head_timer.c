/**
 * @file
 * @brief Multi-threads Timer handling.
 *
 * @date 22.07.10
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 * @author Fedor Burdun
 */

#include <assert.h>
#include <mem/misc/pool.h>
#include <hal/clock.h>
#include <lib/list.h>
#include <embox/unit.h>
#include <kernel/timer.h>
//#include <kernel/thread/event.h>
//#include <kernel/thread/sched.h>
#include <kernel/clock_source.h>
#include <hal/ipl.h>
#include <assert.h>
#include <errno.h>

#define TIMER_POOL_SZ 20 /**<system timers quantity */

EMBOX_UNIT_INIT(unit_init);

static clock_t sys_ticks; /* ticks after start system. */

clock_t clock(void) {
	return sys_ticks;
}

#if 0 //need or not
//static ipl_t timer_ipl;
//static bool timer_in_section = false;
#endif

static inline void timer_safe_section_init(void) {
}

static inline void timer_safe_section_start(void) {
#if 0
	timer_ipl = ipl_save();
	if(false != timer_in_section) {
		return;
	}
	timer_in_section = true;
#endif
}

static inline void timer_safe_section_end(void) {
#if 0
	timer_in_section = false;
	ipl_restore(timer_ipl);
#endif
}

POOL_DEF(timer_pool, sys_timer_t, TIMER_POOL_SZ); //TODO: new allocator (objalloc)

static struct list_head sys_timers_list; /* list head to timers */

static inline void timer_insert_into_list(struct sys_timer *tmr) {
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

int timer_set(struct sys_timer **ptimer, uint32_t ticks,	sys_timer_handler_t handler,
		void *param) {

	if (NULL == handler || NULL == ptimer) {
		return -EINVAL;
	}
	if (NULL == (*ptimer = (sys_timer_t*) pool_alloc(&timer_pool))) {
		return -ENOMEM;
	}
	/* we know that init will be success (right ptimer and handler) */
	timer_init(*ptimer, ticks, handler, param);
	(*ptimer)->is_preallocated = true;

	return 0;
}

int timer_init(struct sys_timer *ptimer, uint32_t ticks,	sys_timer_handler_t handler,
		void *param) {

	if (NULL == handler || NULL == ptimer) {
		return -EINVAL; /* wrong parameters */
	}

	ptimer->is_preallocated = false;
	ptimer->cnt    = ptimer->load = ticks;
	ptimer->handle = handler;
	ptimer->param  = param;

	timer_safe_section_start();
	timer_insert_into_list(ptimer);
	timer_safe_section_end();

	return 0;
}

int timer_close(sys_timer_t *ptimer) {
	if(NULL == ptimer) {
		return -EINVAL;
	}

	timer_safe_section_start();

	list_del(&ptimer->lnk);

	timer_safe_section_end();

	if (ptimer->is_preallocated) {
		pool_free(&timer_pool, ptimer);
	}

	return 0;
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

		timer_insert_into_list(timer);
	}
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
static inline void inc_sys_timers(void) {
	timer_safe_section_start();

	if(timers_need_schedule()) {
		timers_schedule();
	}

	timer_safe_section_end();
}

/**
 * Handling of the clock tick.
 */
void clock_tick_handler(int irq_num, void *dev_id) {
	sys_ticks++;
	inc_sys_timers();
}

/**
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
static int unit_init(void) {
	sys_ticks = 0;
	INIT_LIST_HEAD(&sys_timers_list);
	clock_init();
	clock_setup(clock_source_get_precision());
	timer_safe_section_init();
	return 0;
}

