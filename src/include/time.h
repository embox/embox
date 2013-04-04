/**
 * @file
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 24.11.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef TIME_H_
#define TIME_H_

#include <sys/types.h>
#include <stddef.h>    /* NULL definition */
#include <sys/cdefs.h>

__BEGIN_DECLS

/* Parameters used to convert the time specific values */
#define MSEC_PER_SEC    1000L
#define USEC_PER_MSEC   1000L
#define NSEC_PER_USEC   1000L
#define USEC_PER_SEC    1000000L
#define NSEC_PER_SEC    1000000000L

struct tm {
	int    tm_sec;   /*Seconds [0,60].*/
	int    tm_min;   /*Minutes [0,59].*/
	int    tm_hour;  /*Hour [0,23].   */
	int    tm_mday;  /*Day of month [1,31]. */
	int    tm_mon;   /*Month of year [0,11]. */
	int    tm_year;  /*Years since 1900 */
	int    tm_wday;  /*Day of week [0,6] (Sunday =0). */
	int    tm_yday;  /*Day of year [0,365]. */
	int    tm_isdst; /*Daylight Savings flag. */
};

struct timespec {
	time_t tv_sec;  /*Seconds */
	long   tv_nsec; /*Nanoseconds */
};

struct itimerspec {
	struct timespec it_interval;  /* Timer period. */
	struct timespec it_value;     /* Timer expiration. */
};

/**
 * Converts the calendar time t into a null-terminated string of the form
 * "Wed Jun 30 21:49:08 1993\n".
 * It uses static allocated buffer which might be overwritten by subsequent
 * calls to any of the date and time functions
 */
extern char *ctime(const time_t *timep);

extern char *ctime_r(const time_t *t, char *buff);

extern struct tm *gmtime(const time_t *timep);
extern struct tm *gmtime_r(const time_t *timep, struct tm *result);

extern time_t mktime(struct tm *tm);

/* convert date and time to a string */
extern char *asctime(const struct tm *timeptr);

extern struct tm *localtime(const time_t *timep);

#define CLOCKS_PER_SEC     1000 //TODO CLOCKS_PER_SEC should receive from clock_getres()

/* clocks from beginning of start system */
extern clock_t clock(void);

#define CLOCK_REALTIME  3
#define TIMER_ABSTIME   2
#define CLOCK_MONOTONIC 1

extern int clock_getres(clockid_t clk_id, struct timespec *res);

extern int clock_gettime(clockid_t clk_id, struct timespec *tp);

extern int clock_settime(clockid_t clk_id, const struct timespec *tp);

/* seconds from beginning of start system */
extern time_t time(time_t *t);

/** Format date and time */
extern size_t strftime(char *s, size_t max, const char *fmt, const struct tm *tm);

extern int nanosleep(const struct timespec *req, struct timespec *rem);

__END_DECLS

#endif /* TIME_H_ */
