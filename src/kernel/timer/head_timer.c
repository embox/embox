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
#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>
#include <kernel/clock_source.h>

EMBOX_UNIT_INIT(unit_init);

#define TIMER_POOL_SZ 20 /**<system timers quantity */

const uint32_t clock_source = 1000;

static volatile uint32_t cnt_sys_time; /**< quantity ms after start system */

#include <hal/ipl.h>
#include <assert.h>

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


POOL_DEF(timer_pool, sys_tmr_t, TIMER_POOL_SZ); //TODO: new allocator (objalloc)

static struct list_head *sys_timers_list; /* list head was allocated by clock driver */

uint32_t cnt_system_time(void) { //move and type //exist function `clock()'. Its function need only for it.
	return cnt_sys_time;
}

static inline void timer_insert_into_list(struct sys_tmr *tmr) {
	struct list_head *iter, *tmp;

	tmr->cnt = tmr->load;

	/* if we haven't any timers we just insert new timer into the list */
	if (list_empty(sys_timers_list)) {
		/* we just add timer to list */
		list_add((struct list_head *)tmr, sys_timers_list);
		return;
	}

	/* find first element that its time bigger than inserting @new_time */
	list_for_each_safe(iter, tmp, sys_timers_list) {
		struct sys_tmr *it_tmr = (struct sys_tmr*) iter;

		if (it_tmr->cnt >= tmr->cnt) {
			/* decrease value of next timer after inserting */
			it_tmr->cnt -= tmr->cnt;
			list_add_tail((struct list_head *)tmr, iter);
			return;
		}
		tmr->cnt -= it_tmr->cnt;

	}

	/* add the latest timer to end of list */
	list_add_tail((struct list_head *)tmr, sys_timers_list);
}

int set_timer(struct sys_tmr **ptimer, uint32_t ticks,	TIMER_FUNC handler,
		void *param) {

	if (NULL == handler || NULL == ptimer) {
		return -1;
	}
	if (NULL == (*ptimer = (sys_tmr_t*) pool_alloc(&timer_pool))) {
		return -1;
	}
	/* we know that init will be success (right ptimer and handler) */
	init_timer(*ptimer, ticks, handler, param);
	(*ptimer)->is_preallocated = true;

	return 0;
}

int init_timer(struct sys_tmr *ptimer, uint32_t ticks,	TIMER_FUNC handler,
		void *param) {

	if (NULL == handler || NULL == ptimer) {
		return -1; /* wrong parameters */
	}

	ptimer->is_preallocated = false;
	ptimer->cnt    = ptimer->load = ticks;
	ptimer->handler = handler;
	ptimer->args  = param;

	timer_safe_section_start();
	timer_insert_into_list(ptimer);
	timer_safe_section_end();

	return 0;
}

int close_timer(sys_tmr_t *ptimer) {
	if(NULL == ptimer) {
		return -1;
	}

	timer_safe_section_start();

	list_del((struct list_head *) ptimer);

	timer_safe_section_end();

	if (ptimer->is_preallocated) {
		pool_free(&timer_pool, ptimer);
	}

	return 0;
}

static inline bool timers_need_schedule(void) {
	if(list_empty(sys_timers_list)) {
		return false;
	}

	if(0 == ((sys_tmr_t*)sys_timers_list->next)->cnt) {
		return true;
	} else {
		((sys_tmr_t*)sys_timers_list->next)->cnt--;
	}

	return false;
}

static inline void timers_schedule(void) {
	struct list_head *iter, *tmp;

	list_for_each_safe(iter, tmp, sys_timers_list) {

		if (0 != ((struct sys_tmr *)iter)->cnt) {
			return;
		}

		((struct sys_tmr *)iter)->handler((struct sys_tmr *)iter,
				((struct sys_tmr *)iter)->args);

		list_del(iter);

		timer_insert_into_list((struct sys_tmr *)iter);
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
	cnt_sys_time++;
	inc_sys_timers();
}

/**
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
static int unit_init(void) {
	cnt_sys_time = 0;
	clock_init();
	clock_setup(clock_source_get_HZ());
	sys_timers_list = clock_source_get_timers_list();
	timer_safe_section_init();
	return 0;
}

