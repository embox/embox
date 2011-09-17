/**
 * @file
 *
 * @brief
 *
 * @date 12.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_TIME_H_
#define SYS_TIME_H_

#include <sys/types.h>

struct timeval {
	time_t		tv_sec;
	useconds_t	tv_usec;
};


#endif /* SYS_TIME_H_ */
