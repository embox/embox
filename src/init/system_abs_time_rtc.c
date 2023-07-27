/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    27.07.2023
 */
#include <time.h>

#include <drivers/rtc.h>

int system_abs_time_init(void) {

#ifndef NO_RTC_SUPPORT
	struct rtc_device *rtc;
	struct tm tm = {0};
    struct timespec timespec = {0};

	rtc = rtc_get_device(NULL);	
	rtc_get_time(rtc, &tm);
    
    timespec.tv_sec = mktime(&tm);
    clock_settime(CLOCK_REALTIME, &timespec);
#endif

    return 0;
}