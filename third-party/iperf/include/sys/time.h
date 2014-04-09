#ifndef IPERF_SYS_TIME_H_
#define IPERF_SYS_TIME_H_

#include_next <sys/time.h>

#include <sys/cdefs.h>

#define ITIMER_REAL 0

__BEGIN_DECLS

extern int setitimer(int which, const struct itimerval *value,
		struct itimerval *ovalue);

__END_DECLS

#endif /* IPERF_SYS_TIME_H_ */
