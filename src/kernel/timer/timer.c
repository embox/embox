/**
 * @file
 *
 * @brief
 *
 * @date 09.04.2012
 * @author Anton Bondarev
 */
#include <errno.h>
#include <mem/misc/pool.h>

#include <kernel/timer.h>

#define TIMER_POOL_SZ 20 /**< system timers quantity */

POOL_DEF(timer_pool, sys_timer_t, TIMER_POOL_SZ);

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
