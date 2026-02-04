/*
 * watchdog.h
 *
 *  Created on: 25th of march, 2014
 *      Author: velib
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include <kernel/time/sys_timer.h>
#include <kernel/time/time.h>

typedef void (*watchdog_handler_t)(struct sys_timer *timer, void *param);

struct watchdog {
	struct sys_timer timer;
	struct timeval time_period;
	watchdog_handler_t handler;
};

typedef struct watchdog watchdog_t;

__BEGIN_DECLS
extern int watchdog_set(watchdog_t *p_wdog, struct timeval *p_timeout, watchdog_handler_t handler);
extern int watchdog_start(watchdog_t *p_wdog);
extern int watchdog_kick(watchdog_t *p_wdog);
extern int watchdog_close(watchdog_t *p_wdog);
__END_DECLS


#endif /* WATCHDOG_H_ */
