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

#include <kernel/time/sys_timer.h>
#include <kernel/time/time.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched.h>
#include <hal/clock.h>
#include <hal/cpu.h>

POOL_DEF(timer_pool, sys_timer_t, OPTION_GET(NUMBER,timer_quantity));

int sys_timer_init(struct sys_timer *tmr, unsigned int flags,
		sys_timer_handler_t handler, void *param) {
	if (!handler || !tmr) {
		return -EINVAL;
	}

#ifdef SMP /* XXX */
	/**
	 * No need to init timer_sharing and state file for
	 * sched_tick_timer
	 */
	if(sched_ticker_get_timer() != (void*)tmr){
		tmr->timer_sharing = NULL;
		tmr->state = 0;
	}
#else
	tmr->state = 0;
#endif	

	tmr->handle = handler;
	tmr->param = param;
	tmr->flags = flags;

	return ENOERR;
}

void sys_timer_start(struct sys_timer *tmr, clock_t jiffies) {

	sys_timer_stop(tmr);

	tmr->load = jiffies;
	tmr->cnt = clock_sys_ticks() + tmr->load;

	sched_lock();
	{
		timer_strat_start(tmr);
	}
	sched_unlock();
}

void sys_timer_stop(struct sys_timer *tmr) {
	sched_lock();
	{
		if (sys_timer_is_started(tmr)) {
			timer_strat_stop(tmr);
		}
	}
	sched_unlock();
}

int sys_timer_init_start(struct sys_timer *tmr, unsigned int flags, clock_t jiffies,
		sys_timer_handler_t handler, void *param) {
	int err;

	if ((err = sys_timer_init(tmr, flags, handler, param))) {
		return err;
	}

	sys_timer_start(tmr, jiffies);

	return ENOERR;
}

int sys_timer_init_start_msec(struct sys_timer *tmr, unsigned int flags, uint32_t msec,
		sys_timer_handler_t handler, void *param) {

	return sys_timer_init_start(tmr, flags, ms2jiffies(msec), handler, param);
}

int sys_timer_set(struct sys_timer **ptimer, unsigned int flags, uint32_t msec,
		sys_timer_handler_t handler, void *param) {
	int err;
	struct sys_timer *tmr;

	if (NULL == handler || NULL == ptimer) {
		return -EINVAL;
	}

	if (NULL == (tmr = (sys_timer_t*) pool_alloc(&timer_pool))) {
		return -ENOMEM;
	}

	if ((err = sys_timer_init_start_msec(tmr, flags, msec, handler, param))) {
		pool_free(&timer_pool, tmr);
		return err;
	}

	sys_timer_set_preallocated(tmr);

	*ptimer = tmr;
	return ENOERR;
}

int sys_timer_close(struct sys_timer *tmr) {
	if (NULL == tmr) {
		return -EINVAL;
	}

	sys_timer_stop(tmr);

	if (sys_timer_is_preallocated(tmr)) {
		sys_timer_clear_preallocated(tmr);
		pool_free(&timer_pool, tmr);
	}

	return ENOERR;
}
