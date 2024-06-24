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

#include <sys/cdefs.h>
#include <sys/select.h>
#include <sys/types.h> /* time_t and suseconds_t */

__BEGIN_DECLS

#define ITIMER_REAL    0x1 /* Decrements in real time. */
#define ITIMER_VIRTUAL 0x2 /* Decrements in process virtual time. */
/* Decrements both in process virtual time and when the system is running on
 * behalf of the process. */
#define ITIMER_PROF    0x4

/* The <sys/time.h> header defines the timeval structure that includes at least
 * the following members:
 * time_t         tv_sec      seconds
 * suseconds_t    tv_usec     microseconds
 */
struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

/* The <sys/time.h> header defines the itimerval structure that includes at
 * least the following members:
 * struct timeval it_interval timer interval
 * struct timeval it_value    current value
 */
struct itimerval {
	struct timeval it_interval; /* Timer period. */
	struct timeval it_value;    /* Timer expiration. */
};

extern int gettimeofday(struct timeval *ts, void *tz);

extern int getitimer(int, struct itimerval *);

extern int setitimer(int which, const struct itimerval *value,
    struct itimerval *ovalue);

/* LEGACY */
extern int utimes(const char *path, const struct timeval times[2]);

/* Only Linux compatible */
struct timezone {
	int tz_minuteswest; /* minutes west of Greenwich */
	int tz_dsttime;     /* type of DST correction */
};

/* gettimeofday is posix function, but settimeofday is not. */
extern int settimeofday(const struct timeval *tv, const struct timezone *tz);

/**
 * timeval operations
 */
extern void timeradd(struct timeval *a, struct timeval *b, struct timeval *r);

extern void timersub(struct timeval *a, struct timeval *b, struct timeval *r);

extern void timerclear(struct timeval *tvp);

extern int timerisset(struct timeval *tvp);

#define timercmp(a, b, CMP)       \
	(((a)->tv_sec CMP(b)->tv_sec) \
	    || (((a)->tv_sec == (b)->tv_sec) && ((a)->tv_usec CMP(b)->tv_usec)))

/* Operations on timespecs. */
#define timespecclear(tsp) (tsp)->tv_sec = (tsp)->tv_nsec = 0
#define timespecisset(tsp) ((tsp)->tv_sec || (tsp)->tv_nsec)
#define timespecisvalid(tsp) \
	((tsp)->tv_nsec >= 0 && (tsp)->tv_nsec < 1000000000L)

#define timespeccmp(tsp, usp, cmp)                                         \
	(((tsp)->tv_sec == (usp)->tv_sec) ? ((tsp)->tv_nsec cmp(usp)->tv_nsec) \
	                                  : ((tsp)->tv_sec cmp(usp)->tv_sec))

#define timespecadd(tsp, usp, vsp)                        \
	do {                                                  \
		(vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;    \
		(vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec; \
		if ((vsp)->tv_nsec >= 1000000000L) {              \
			(vsp)->tv_sec++;                              \
			(vsp)->tv_nsec -= 1000000000L;                \
		}                                                 \
	} while (0)

#define timespecsub(tsp, usp, vsp)                        \
	do {                                                  \
		(vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;    \
		(vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec; \
		if ((vsp)->tv_nsec < 0) {                         \
			(vsp)->tv_sec--;                              \
			(vsp)->tv_nsec += 1000000000L;                \
		}                                                 \
	} while (0)

#define TIMEVAL_TO_TIMESPEC(tv, ts)           \
	do {                                      \
		(ts)->tv_sec = (tv)->tv_sec;          \
		(ts)->tv_nsec = (tv)->tv_usec * 1000; \
	} while (0)

#define TIMESPEC_TO_TIMEVAL(tv, ts)           \
	do {                                      \
		(tv)->tv_sec = (ts)->tv_sec;          \
		(tv)->tv_usec = (ts)->tv_nsec / 1000; \
	} while (0)

__END_DECLS

#endif /* COMPAT_POSIX_SYS_TIME_H_ */
