/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    17.01.2014
 */

#ifndef IPERF_EMBOX_COMPAT_H_
#define IPERF_EMBOX_COMPAT_H_

#include <sys/types.h>
#include <sys/time.h>

#define ITIMER_REAL 0

static inline int setitimer(int which, const struct itimerval *new_value,\
		struct itimerval *old_value) {
	return -1;
}
//#include <syslog.h>
#define LOG_DAEMON    100

//#include <unistd.h>
static inline pid_t setsid(void) {
	return -1;
}

//#include <libstd.h>
static inline int atexit(void (*function)(void)) {
	return -1;
}

#define HAVE_GETTIMEOFDAY

#define rpl_malloc malloc

#endif /* IPERF_EMBOX_COMPAT_H_ */
