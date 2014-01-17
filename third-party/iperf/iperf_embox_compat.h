/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    17.01.2014
 */

#ifndef IPERF_EMBOX_COMPAT_H_
#define IPERF_EMBOX_COMPAT_H_

#include <sys/time.h>

#define ITIMER_REAL 0

static inline int setitimer(int which, const struct itimerval *new_value,\
		struct itimerval *old_value) {
	return -1;
}


#endif /* IPERF_EMBOX_COMPAT_H_ */
