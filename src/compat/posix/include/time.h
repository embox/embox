/**
 * @file
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 24.11.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef POSIX_TIME_H_
#define POSIX_TIME_H_

#include <sys/types.h>
#include <sys/time.h>
#include <stddef.h>    /* NULL definition */
#include <signal.h>


struct tm {
	int    tm_sec;   /*Seconds [0,60].*/
	int    tm_min;   /*Minutes [0,59].*/
	int    tm_hour;  /*Hour [0,23].   */
	int    tm_mday;  /*Day of month [1,31]. */
	int    tm_mon;   /*Month of year [0,11]. */
	int    tm_year;  /*Years since 1900.  */
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

#define CLOCKS_PER_SEC     1000 //TODO CLOCKS_PER_SEC should receive from clock_getres()

/* clocks from beginning of start system */
extern clock_t clock(void);

/* seconds from beginning of start system */
extern time_t time(time_t *);

extern int clock_getres(clockid_t clk_id, struct timespec *res);

extern int clock_gettime(clockid_t clk_id, struct timespec *tp);

extern int clock_settime(clockid_t clk_id, const struct timespec *tp);


#endif /* POSIX_TIME_H_ */
