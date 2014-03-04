/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.12.2013
 */

#include "host_defs.h"
#include <assert.h>
#include <stddef.h>
#include <sys/time.h>

static HOST_FNX(int, setitimer,
		CONCAT(int what, const struct itimerval *it, struct itimerval *oit),
		what, it, oit)

void host_timer_config(int usec) {
	struct itimerval itv = {
		.it_interval = {
			.tv_sec = 0,
			.tv_usec = usec,
		},
		.it_value = {
			.tv_sec = 0,
			.tv_usec = usec,
		},
	};
	int res;

	res = host_setitimer(ITIMER_REAL, &itv, NULL);
	assert(res == 0);
}
