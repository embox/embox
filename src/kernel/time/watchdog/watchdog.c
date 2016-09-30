/**
 * watchdog.c
 *
 *  Created on: 25th of march, 2014
 *      Author: velib
 */

#include <kernel/time/watchdog.h>
#include <kernel/time/timer.h>
#include <defines/null.h>


int watchdog_set(watchdog_t *p_wdog, struct timeval *p_timeout, watchdog_handler_t handler)
{
	p_wdog->time_period.tv_sec = p_timeout->tv_sec;
	p_wdog->time_period.tv_usec = p_timeout->tv_usec;
	p_wdog->handler = handler;

	return 0;
}


int watchdog_start(watchdog_t *p_wdog)
{
	return timer_init_start_msec(&(p_wdog->timer), 0, timeval_to_ms(&(p_wdog->time_period)),
			p_wdog->handler, NULL);
}


int watchdog_kick(watchdog_t *p_wdog)
{
	timer_close(&(p_wdog->timer));
	return watchdog_start(p_wdog);
}


int watchdog_close(watchdog_t *p_wdog)
{
	p_wdog->time_period.tv_usec = 0;
	p_wdog->time_period.tv_sec = 0;
	p_wdog->handler = NULL;

	return timer_close(&(p_wdog->timer));
}

