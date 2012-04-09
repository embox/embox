/**
 * @file
 *
 * @brief
 *
 * @date 09.04.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>
#include <errno.h>
#include <mem/misc/pool.h>

#include <hal/clock.h>
#include <kernel/clock_source.h>
#include <kernel/timer.h>

#define TIMER_POOL_SZ 20 /**< system timers quantity */

EMBOX_UNIT_INIT(module_init);

POOL_DEF(timer_pool, sys_timer_t, TIMER_POOL_SZ);

static clock_t sys_ticks; /* ticks after start system. */

clock_t clock(void) {
	return sys_ticks;
}

/**
 * Handling of the clock tick.
 */
void clock_tick_handler(int irq_num, void *dev_id) {
	sys_ticks++;
	timer_sched();
}


int timer_init(sys_timer_t *ptimer, uint32_t ticks,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !ptimer) {
		return -EINVAL;
	}

	ptimer->is_preallocated = false;
	ptimer->cnt    = ptimer->load = ticks;
	ptimer->handle = handler;
	ptimer->param  = param;

	timer_start(ptimer);

	return 0;
}

int timer_set(sys_timer_t **ptimer, uint32_t ticks,
		sys_timer_handler_t handler, void *param) {

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

int timer_close(sys_timer_t *ptimer) {
	if (NULL == ptimer) {
		return -EINVAL;
	}

	timer_stop(ptimer);
	if (ptimer->is_preallocated) {
		pool_free(&timer_pool, ptimer);
	}

	return 0;
}


/**
 * Initialization of the timer subsystem.
 *
 * @return 0 if success
 */
static int module_init(void) {
	sys_ticks = 0;
	clock_init();
	clock_setup(clock_source_get_precision(NULL));
	return 0;
}
