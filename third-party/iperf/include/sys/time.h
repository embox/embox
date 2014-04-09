#ifndef IPERF_SYS_TIME_H_
#define IPERF_SYS_TIME_H_

#include_next <sys/time.h>

#define ITIMER_REAL 0

extern int setitimer(int which, const struct itimerval *value,
		struct itimerval *ovalue);

#endif /* IPERF_SYS_TIME_H_ */
