/**
 * @file
 * @brief
 *
 * @date 12.09.11
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_SYS_TIME_H_
#define COMPAT_POSIX_SYS_TIME_H_



/*The time_t and suseconds_t types are defined as described in <sys/types.h>.*/
#include <defines/time_t.h>
#include <defines/suseconds_t.h>


/* The <sys/time.h> header defines the timeval structure that includes at least
 * the following members:
 * time_t         tv_sec      seconds
 * suseconds_t    tv_usec     microseconds
 */
struct timeval {
	time_t      tv_sec;
	suseconds_t tv_usec;
};

/* The <sys/time.h> header defines the itimerval structure that includes at
 * least the following members:
 * struct timeval it_interval timer interval
 * struct timeval it_value    current value
 */
struct itimerval {
	struct timeval it_interval;  /* Timer period. */
	struct timeval it_value;     /* Timer expiration. */
};


/* TODO this is only for Linux */
struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
};

#include <sys/cdefs.h>

__BEGIN_DECLS

/* TODO Linux specific signature
extern int gettimeofday(struct timeval *ts, struct timezone *tz);
*/

extern int gettimeofday(struct timeval *ts, void *tz);


__END_DECLS

#endif /* COMPAT_POSIX_SYS_TIME_H_ */
