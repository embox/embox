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

int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue) {
	return SET_ERRNO(ENOSYS);
}