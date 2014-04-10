/*
 * watchdog.c
 *
 *  Created on: 25 марта 2014 г.
 *      Author: velib
 */

#include <kernel/time/watchdog.h>
#include <kernel/time/timer.h>
#include <defines/null.h>

// Easy way to send the signals
#include <kernel/printk.h>

static void time_is_out(struct sys_timer *timer, void *param) {
	printk("Timeout occured, watchdog wasn't kicked!\n");
}


int watchdog_set(watchdog_t *p_wdog, struct timeval *p_timeout)
{
	p_wdog ->  time_period.tv_sec = p_timeout -> tv_sec;
	p_wdog ->  time_period.tv_usec = p_timeout -> tv_usec;

	/*****/
	return timer_set(&(p_wdog -> timer), 0, timeval_to_ms(p_timeout),
			&time_is_out, NULL);
	/*****/

}


void watchdog_start(watchdog_t *p_wdog)
{
	if (timer_is_preallocated(p_wdog -> timer))
		return timer_set_started(p_wdog -> timer);
}


void watchdog_stop(watchdog_t *p_wdog)
{
	if (timer_is_started(p_wdog -> timer))
		timer_set_stopped(p_wdog -> timer);

	return timer_set_preallocated(p_wdog -> timer);
}


void watchdog_kick(watchdog_t *p_wdog)
{
	watchdog_stop(p_wdog);
/*****/
	printk("Watchdog was kicked!\n");
/*****/
	return watchdog_start(p_wdog);
}


int watchdog_close(watchdog_t *p_wdog)
{
	p_wdog -> time_period.tv_usec = 0;
	p_wdog -> time_period.tv_sec = 0;

	return timer_close(p_wdog -> timer);
}

