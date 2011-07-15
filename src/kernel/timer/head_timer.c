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

EMBOX_UNIT_INIT(timer_init);

#if 1
#define DOUT prom_printf
#else
#define DOUT(a,...)
#endif

/* What will be, when in set_@timer (e.g inside safe section) in handle_function be run @set_timer?! */
const uint32_t clock_source = 1000;

struct sys_tmr {
	struct list_head *next, *prev;
	uint32_t   load;
	uint32_t   cnt;
	TIMER_FUNC handle;
	void       *param;
	bool isLive;
/*	uint32_t   flags; // periodical or not */
};

static volatile uint32_t cnt_sys_time; /**< quantity ms after start system */

#include <hal/ipl.h>
#include <assert.h>

static __ipl_t timer_ipl;
static bool timer_in_section = false;

static inline void timer_safe_section_init(void) {
}

static inline void timer_safe_section_start(void) {
	return;
	assert(!timer_in_section);
	timer_in_section = true;
	timer_ipl = ipl_save();

}

static inline void timer_safe_section_end(void) {
	return;
	ipl_restore(timer_ipl);
	timer_in_section = false;

}


POOL_DEF(timer_pool, sys_tmr_t, TIMER_POOL_SZ);
static struct list_head *sys_timers_list;

uint32_t cnt_system_time(void) {
	return cnt_sys_time;
}

int set_timer(sys_tmr_ptr *ptimer, uint32_t ticks,
		TIMER_FUNC handle, void *param) {
	sys_tmr_ptr new_timer;

	struct list_head *tmp, *tmp2;
	sys_tmr_ptr tmr;
	timer_safe_section_start();

	if (!handle || !(new_timer = (sys_tmr_ptr) pool_alloc(&timer_pool))) {
		if (ptimer) { /* it's necessary? */
			*ptimer = NULL;
		}
		return 1;
	}
	new_timer->cnt    = new_timer->load = ticks;
	new_timer->handle = handle;
	new_timer->param  = param;
	new_timer->isLive = true;

	/*
	 * find timer before that need paste @new_timer
	 */
	tmr = NULL;
	/* find first element that its time bigger than inserting @new_time */
	list_for_each_safe(tmp, tmp2, sys_timers_list) {
		tmr = (sys_tmr_ptr) tmp;
		if (tmr->cnt >= new_timer->cnt) {
			break;
		}
		new_timer->cnt -= tmr->cnt;
	}
	if (tmr) {
		tmr->cnt -= new_timer->cnt;
		/* will it realy paste after head and before tmr? -- check it */
		list_add_tail( (struct list_head*) new_timer, (struct list_head*) tmr);
		DOUT("added: %d %d\n", new_timer->cnt, tmr->cnt);
	} else { /* list is empty */
		DOUT("added: %d\n", new_timer->cnt);
		list_add_tail( (struct list_head*) new_timer, sys_timers_list);
	}

	if (ptimer) {
		*ptimer = new_timer;
	}

	timer_safe_section_end();

	return 0;
}

int close_timer(sys_tmr_ptr *ptimer) {
	(*ptimer)->isLive = false;
	return 0;
	if (ptimer) {
		/* fix next timer */
		if (((struct list_head*)*ptimer)->next != sys_timers_list) {
			((sys_tmr_ptr)
				((struct list_head*)*ptimer)->next)
					->cnt += (*ptimer)->cnt;
		}

		list_del((struct list_head *) *ptimer);
		pool_free(&timer_pool, *ptimer);
		*ptimer = NULL;
	}
	return 0;
}

/**
 * For each timer in the timers array do the following: if the timer is enable
 * and the counter of this timer is the zero then its initial value is assigned
 * to the counter and the function is executed.
 */
static void inc_sys_timers(void) {
	struct list_head *tmp, *tmp2;
	sys_tmr_ptr tmr;

	timer_safe_section_start();
	if (!list_empty(sys_timers_list)) {
		/* first element in list */
		tmr = ((sys_tmr_ptr) sys_timers_list->next);

		if(tmr->cnt) { /* can be zero */
			--(tmr->cnt);
		}

		DOUT("#1 %d\n",(((sys_tmr_ptr) sys_timers_list->next)->cnt));

		list_for_each_safe(tmp, tmp2, sys_timers_list) {
			tmr = (sys_tmr_ptr) tmp;
			DOUT("foreach: %d\n",tmr->cnt);
			if (0 == tmr->cnt) {
				uint32_t load = tmr->load;
				void* args = tmr->param;
				TIMER_FUNC handle = tmr->handle;

				if (tmr->isLive ) {
					tmr->handle(tmr, tmr->param);
				}

				/* repaste in list */
				list_del((struct list_head *) tmr);
				pool_free(&timer_pool, tmr);

				set_timer(NULL,load,handle,args);
			} else {
				break;
			}
		}
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
int timer_init(void) {
	cnt_sys_time = 0;
	clock_init();
	clock_setup(clock_source_get_HZ());
	sys_timers_list = clock_source_get_timers_list();
	timer_safe_section_init();
	return 0;
}
