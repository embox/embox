/**
 * @file
 * @brief Multi-threads Timer handling.
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
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

#if 0
#define DOUT prom_printf
#else
#define DOUT(a,...)
#endif

const uint32_t clock_source = 1000;

static volatile uint32_t cnt_sys_time; /**< quantity ms after start system */

#include <hal/ipl.h>
#include <assert.h>

static ipl_t timer_ipl;
static bool timer_in_section = false;

static inline void timer_safe_section_init(void) {
}

static inline void timer_safe_section_start(void) {
	timer_ipl = ipl_save();
	if(false != timer_in_section) {
		return;
	}
	timer_in_section = true;
}

static inline void timer_safe_section_end(void) {
	timer_in_section = false;
	ipl_restore(timer_ipl);
}


POOL_DEF(timer_pool, sys_tmr_t, TIMER_POOL_SZ);
static struct list_head *sys_timers_list;

uint32_t cnt_system_time(void) {
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
		if (((struct sys_tmr *)iter)->cnt >= tmr->cnt) {
			/* decrease value of next timer after inserting */
			((struct sys_tmr *)iter)->cnt -= tmr->cnt;
			list_add_tail((struct list_head *)tmr, iter);
			return;
		}
		tmr->cnt -= ((struct sys_tmr *)iter)->cnt;

	}

	/* add the latest timer to end of list */
	if (iter->next == sys_timers_list) {
		list_add((struct list_head *)tmr, iter);
		return;
	}

}

/**
 * @param ptimer is double pointer to timer struct. It can be NULL if we don't need to know and use sys_tmr_t
 * after set timer.
 * @param handler will be called after @param ticks time with @param args
 * @return is zero if setting succeed and nonzero otherwise.
 */
int set_timer(struct sys_tmr **ptimer, uint32_t ticks,	TIMER_FUNC handler,
		void *args) {
	struct sys_tmr *new_timer;

	if (NULL == handler) {
		return -1; /* wrong parameters */
	}

	if (NULL == (new_timer = (sys_tmr_t*)pool_alloc(&timer_pool))) {
		if (ptimer) {
			*ptimer = NULL;
		}
		return -1;
	}

	new_timer->is_preallocated = true;
	new_timer->cnt    = new_timer->load = ticks;
	new_timer->handler = handler;
	new_timer->args  = args;

	timer_safe_section_start();
	timer_insert_into_list(new_timer);
	timer_safe_section_end();

	if (ptimer) {
		*ptimer = new_timer;
	}

	return 0;
}

int init_timer(struct sys_tmr **ptimer, uint32_t ticks,	TIMER_FUNC handler,
		void *args) {
	struct sys_tmr *new_timer = *ptimer;

	if (NULL == handler || !new_timer) {
		return -1; /* wrong parameters */
	}

	new_timer->is_preallocated = false;
	new_timer->cnt    = new_timer->load = ticks;
	new_timer->handler = handler;
	new_timer->args  = args;

	timer_safe_section_start();
	timer_insert_into_list(new_timer);
	timer_safe_section_end();

	if (ptimer) {
		*ptimer = new_timer;
	}

	return 0;
}

int close_timer(sys_tmr_t **ptimer) {
	timer_safe_section_start();
	list_del((struct list_head *) *ptimer);
	timer_safe_section_end();
	if ((*ptimer)->is_preallocated) {
		pool_free(&timer_pool, *ptimer);
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
static void inc_sys_timers(void) {
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

