/**
 * @file
 * @brief
 *
 * @date 29.04.25
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>

#include <sys/timeb.h>

int ftime(struct timeb *timeb) {
	struct timeval tv;

	gettimeofday(&tv, NULL);
	timeb->time     = tv.tv_sec;
	timeb->millitm  = tv.tv_usec;
	timeb->timezone = 0;
	timeb->dstflag  = 0;

	return 0;
}