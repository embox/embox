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
struct itimerspec {
	struct timeval it_interval;  /* Timer period. */
	struct timeval it_value;     /* Timer expiration. */
};



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
#include <assert.h>
#include <time.h>

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
