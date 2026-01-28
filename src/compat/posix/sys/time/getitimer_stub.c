/**
 * @file
 *
 * @brief Interval timers stub
 *
 * @date 28.01.26
 * @author Dmitry Pilyuk
 */

#include <errno.h>
#include <sys/time.h>

int getitimer(int which, struct itimerval *value) {
	return SET_ERRNO(ENOSYS);
}