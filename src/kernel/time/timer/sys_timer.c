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
#include <kernel/sched/sched_lock.h>
#include <util/lang.h>

POOL_DEF(timer_pool, sys_timer_t, OPTION_GET(NUMBER,timer_quantity));

int timer_init(struct sys_timer *tmr, unsigned int flags, clock_t jiffies,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !tmr) {
		return -EINVAL;
	}

	tmr->state = 0;
	tmr->handle = handler;
	tmr->param = param;
	tmr->flags = flags;

	if (timer_is_periodic(tmr)) {
		tmr->load = jiffies;
		tmr->cnt = tmr->load + 1;
	} else {
		tmr->cnt = tmr->load = jiffies + 1;
	}

	sched_lock();
	{
		timer_strat_start(tmr);
	}
	sched_unlock();

	return ENOERR;
}

int timer_init_msec(struct sys_timer *tmr, unsigned int flags, uint32_t msec,
		sys_timer_handler_t handler, void *param) {

	return timer_init(tmr, flags, ms2jiffies(msec), handler, param);
}

int timer_set(struct sys_timer **ptimer, unsigned int flags, uint32_t msec,
		sys_timer_handler_t handler, void *param) {

	if (NULL == handler || NULL == ptimer) {
		return -EINVAL;
	}
	if (NULL == (*ptimer = (sys_timer_t*) pool_alloc(&timer_pool))) {
		return -ENOMEM;
	}
	/* we know that init will be success (right ptimer and handler) */
	timer_init_msec(*ptimer, flags, msec, handler, param);
	timer_set_preallocated(*ptimer);

	return ENOERR;
}

int timer_close(struct sys_timer *tmr) {
	if (NULL == tmr) {
		return -EINVAL;
	}
	if (timer_is_started(tmr)) {
		sched_lock();
		{
			timer_strat_stop(tmr);
		}
		sched_unlock();
	}
	if (timer_is_preallocated(tmr)) {
		pool_free(&timer_pool, tmr);
	}

	return ENOERR;
}
