/**
 * @file
 *
 * @brief
 *
 * @date 09.04.2012
 * @author Anton Bondarev
 */
#include <errno.h>
#include <embox/unit.h>
#include <mem/misc/pool.h>

#include <kernel/timer.h>

POOL_DEF(timer_pool, sys_timer_t, OPTION_GET(NUMBER,timer_quantity));

int timer_init(struct sys_timer *tmr, uint32_t ticks,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !tmr) {
		return -EINVAL;
	}

	tmr->is_preallocated = false;
	tmr->cnt = tmr->load = ticks;
	tmr->handle = handler;
	tmr->param = param;

	timer_strat_start(tmr);

	return ENOERR;
}

int timer_set(struct sys_timer **ptimer, uint32_t ticks,
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

	return ENOERR;
}

int timer_close(struct sys_timer *tmr) {
	if (NULL == tmr) {
		return -EINVAL;
	}

	timer_strat_stop(tmr);
	if (tmr->is_preallocated) {
		pool_free(&timer_pool, tmr);
	}

	return ENOERR;
}
