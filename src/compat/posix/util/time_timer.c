/**
 * @file
 *
 * @brief POSIX per-process timers (REALTIME) 
 *
 * @date 24.04.25
 * @author Anton Bondarev
 */

#include <time.h>

int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid) {
	return 0;
}

int timer_settime(timer_t timerid, int flags, const struct itimerspec *value,
    struct itimerspec *ovalue) {
	return 0;
}

int timer_gettime(timer_t timerid, struct itimerspec *value) {
	return 0;
}

int timer_getoverrun(timer_t timerid) {
	return 0;
}
