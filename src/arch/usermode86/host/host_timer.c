/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.12.2013
 */

#include <assert.h>
#include <stddef.h>
#include <sys/time.h>

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

	res = setitimer(ITIMER_REAL, &itv, NULL);
	assert(res == 0);
}
