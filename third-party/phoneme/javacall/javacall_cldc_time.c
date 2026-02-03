/**
 * @file
 * @brief
 *
 * @date 7.01.13
 * @author Alexander Kalmuk
 */

#include <javacall_time.h>
#include <kernel/time/timer.h>
#include <kernel/time/time.h>
#include <time.h>
#include <hal/clock.h>
#include <unistd.h>
#include <errno.h>
#include "embox_java_compat.h"

#define SECONDS_1900_1970 2208988800L

void javacall_time_sleep(javacall_uint64 ms){
	usleep((useconds_t) ms);
}

javacall_result javacall_time_initialize_timer(
                    int                      wakeupInMilliSecondsFromNow,
                    javacall_bool            cyclic,
                    javacall_callback_func   func,
					/*OUT*/ javacall_handle	*handle) {
	int res, is_cyclic;
	sys_timer_t *timer;

	is_cyclic = (cyclic == JAVACALL_TRUE ? SYS_TIMER_PERIODIC : SYS_TIMER_ONESHOT);

	if (0 > (res = sys_timer_set(&timer, is_cyclic, wakeupInMilliSecondsFromNow,
			(sys_timer_handler_t)func, NULL))) {
		return emboxErrno2javaErrno(res);
	}

	*handle = (void *)timer;

    return JAVACALL_OK;
}

javacall_int64 javacall_time_get_milliseconds_since_1970(void) {
	struct timeval tv;

	gettimeofday(&tv, NULL);

    return (((javacall_int64) (tv.tv_sec - SECONDS_1900_1970))*1000 + tv.tv_usec/1000);
}

javacall_time_milliseconds /*OPTIONAL*/ javacall_time_get_clock_milliseconds(void){
	return (javacall_time_milliseconds) jiffies2ms(clock());
}

javacall_result javacall_time_finalize_timer(javacall_handle handle) {
	int res;

	if (NULL == handle) {
		return JAVACALL_FAIL;
	}

	if (0 > (res = sys_timer_close((sys_timer_t *)handle))) {
		return emboxErrno2javaErrno(res);
	}

    return JAVACALL_OK;
}

void javacall_time_suspend_ticks(javacall_handle handle) {
	//timer_suspend((sys_timer_t *)handle);
}

void javacall_time_resume_ticks(javacall_handle handle){
	//timer_resume((sys_timer_t *)handle);
}

javacall_int64 /*OPTIONAL*/ javacall_time_get_monotonic_clock_counter(void) {
	return clock();
}

javacall_int64 /*OPTIONAL*/ javacall_time_get_monotonic_clock_frequency(void) {
	return clock_freq();
}
