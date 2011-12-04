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
#include <sys/time.h>

#define MILLISEC_PER_SEC 1000
#define MICROSEC_PER_SEC 1000000


extern char *ctime(const time_t *timep, char *buff);

/* clocks from beginning of start system */
extern clock_t clock(void);

/* time in struct timeval from start of system */
extern struct timeval * get_timeval(struct timeval *);

/* seconds from beginning of start system */
extern time_t time(time_t *);

#endif /* TIME_H_ */
