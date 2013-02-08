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

#include <kernel/time/timer.h>
#include <kernel/time/time.h>

POOL_DEF(timer_pool, sys_timer_t, OPTION_GET(NUMBER,timer_quantity));

int timer_init_ns(struct sys_timer *tmr, unsigned int flags, uint32_t nsec,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !tmr) {
		return -EINVAL;
	}

	tmr->state = 0;
	tmr->cnt = tmr->load = ns2jiffies(nsec);
	tmr->handle = handler;
	tmr->param = param;
	tmr->flags = flags;

	timer_strat_start(tmr);

	return ENOERR;
}

int timer_init_us(struct sys_timer *tmr, unsigned int flags, uint32_t usec,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !tmr) {
		return -EINVAL;
	}

	tmr->state = 0;
	tmr->cnt = tmr->load = us2jiffies(usec);
	tmr->handle = handler;
	tmr->param = param;
	tmr->flags = flags;

	timer_strat_start(tmr);

	return ENOERR;
}

int timer_init_ms(struct sys_timer *tmr, unsigned int flags, uint32_t msec,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !tmr) {
		return -EINVAL;
	}

	tmr->state = 0;
	tmr->cnt = tmr->load = ms2jiffies(msec);
	tmr->handle = handler;
	tmr->param = param;
	tmr->flags = flags;

	timer_strat_start(tmr);

	return ENOERR;
}

int timer_set(struct sys_timer **ptimer, unsigned int flags, uint32_t usec,
		sys_timer_handler_t handler, void *param) {

	if (NULL == handler || NULL == ptimer) {
		return -EINVAL;
	}
	if (NULL == (*ptimer = (sys_timer_t*) pool_alloc(&timer_pool))) {
		return -ENOMEM;
	}
	/* we know that init will be success (right ptimer and handler) */
	timer_init_ms(*ptimer, flags, usec, handler, param);
	timer_set_preallocated(*ptimer);

	return ENOERR;
}

int timer_close(struct sys_timer *tmr) {
	if (NULL == tmr) {
		return -EINVAL;
	}
	if(timer_is_started(tmr)) {
		timer_strat_stop(tmr);
	}
	if (timer_is_preallocated(tmr)) {
		pool_free(&timer_pool, tmr);
	}

	return ENOERR;
}
