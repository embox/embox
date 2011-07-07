/**
 * @file
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 24.11.09
 * @author Anton Bondarev
 */

#ifndef TIME_H_
#define TIME_H_

#include <sys/types.h>

#define MILLISEC_PER_SEC 1000
#define MICROSEC_PER_SEC 1000000

typedef __time_t time_t;

struct timeval {
	time_t		tv_sec;
	useconds_t	tv_usec;
};

extern char *ctime(const time_t *timep, char *buff);

/* clocks from beginning of start system */
extern clock_t clock(void);

/* time in struct timeval from start of system */
extern struct timeval * get_timeval(struct timeval *);

/* secondss from beginning of start system */
extern time_t time(time_t *);

#endif /* TIME_H_ */
