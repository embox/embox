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



#include <sys/cdefs.h>

__BEGIN_DECLS

extern int gettimeofday(struct timeval *ts, void *tz);

/* TODO this is only for Linux */
struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
};

/* TODO Linux specific signature
extern int gettimeofday(struct timeval *ts, struct timezone *tz);
*/

//TODO only Linux compatible
/**
 * timeval operations
 */

extern void timeradd(struct timeval *a, struct timeval *b, struct timeval *r);

extern void timersub(struct timeval *a, struct timeval *b, struct timeval *r);

extern void timerclear(struct timeval *tvp);

extern int timerisset(struct timeval *tvp);

#define timercmp(a, b, CMP) \
	(((a)->tv_sec CMP (b)->tv_sec) \
		|| (((a)->tv_sec == (b)->tv_sec) \
			&& ((a)->tv_usec CMP (b)->tv_usec)))


__END_DECLS

#include <sys/select.h>

#endif /* COMPAT_POSIX_SYS_TIME_H_ */
