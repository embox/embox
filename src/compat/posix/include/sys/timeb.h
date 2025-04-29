/**
 * @file
 * @brief
 *
 * @date 29.04.202025
 * @author Anton Bondarev
 */

#ifndef COMPAT_POSIX_SYS_TIMEB_H_
#define COMPAT_POSIX_SYS_TIMEB_H_

#include <sys/cdefs.h>

#include <sys/types.h> /* time_t */

struct timeb {
	time_t         time;     /* The seconds portion of the current time.  */
	unsigned short millitm;  /* The milliseconds portion of the current time. */
	short          timezone; /* The local timezone in minutes west of Greenwich. */
	short          dstflag;  /* TRUE if Daylight Savings Time is in effect.*/
};

__BEGIN_DECLS

extern int ftime(struct timeb *);

__END_DECLS

#endif /* COMPAT_POSIX_SYS_TIMEB_H_ */
