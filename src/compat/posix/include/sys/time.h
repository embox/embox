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

#include <assert.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
};

extern int gettimeofday(struct timeval *ts, struct timezone *tz);

/**
 * timeval operations
 */
static inline void timeradd(struct timeval *a, struct timeval *b,
		struct timeval *result) {
	assert((a != NULL) && (b != NULL) && (result != NULL));
	result->tv_sec = a->tv_sec + b->tv_sec;
	result->tv_usec = a->tv_usec + b->tv_usec;
	if (result->tv_usec >= USEC_PER_SEC) {
		++result->tv_sec;
		result->tv_usec -= USEC_PER_SEC;
	}
}

static inline void timersub(struct timeval *a, struct timeval *b,
		struct timeval *result) {
	assert((a != NULL) && (b != NULL) && (result != NULL));
	result->tv_sec = a->tv_sec - b->tv_sec;
	result->tv_usec = a->tv_usec - b->tv_usec;
	if (result->tv_usec < 0) {
		--result->tv_sec;
		result->tv_usec += USEC_PER_SEC;
	}
}

static inline void timerclear(struct timeval *tvp) {
	assert(tvp != NULL);
	tvp->tv_sec = 0;
	tvp->tv_usec = 0;
}

static inline int timerisset(struct timeval *tvp) {
	assert(tvp != NULL);
	return (tvp->tv_sec != 0) || (tvp->tv_usec != 0);
}

#define timercmp(a, b, CMP) \
	(((a)->tv_sec CMP (b)->tv_sec) \
		|| (((a)->tv_sec == (b)->tv_sec) \
			&& ((a)->tv_usec CMP (b)->tv_usec)))

__END_DECLS

#endif /* COMPAT_POSIX_SYS_TIME_H_ */
