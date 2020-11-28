/**
 * @file
 * @brief System time correction with NTP
 *
 * @date 25.07.2012
 * @author Alexander Kalmuk
 */
#include <time.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/itimer.h>

#include <drivers/rtc.h>

static struct timespec timekeep_g_time;
static struct itimer timekeep_g_itimer;

#ifndef NO_RTC_SUPPORT
static void rtc_update(time_t time) {

	struct rtc_device *rtc;
	struct tm tm = {0};
	time_t sec;

	rtc = rtc_get_device(NULL);
	sec = time;
	gmtime_r(&sec, &tm);
	rtc_set_time(rtc, &tm);
}
#else
#define rtc_update(time)
#endif

void setnsofday(const struct timespec *newtime, const struct timezone *tz) {
	timekeep_g_time = *newtime;
	itimer_init(&timekeep_g_itimer, timekeep_g_itimer.cs, 0);

	rtc_update(newtime->tv_sec);
}

void getnsofday(struct timespec *t, struct timezone *tz) {
	struct timespec ts;

	itimer_read_timespec(&timekeep_g_itimer, &ts);
	*t = timespec_add(timekeep_g_time, ts);
}

int realtime_clock_select(void) {
	extern struct clock_source *kernel_clock_source;

	timekeep_g_time.tv_sec = 0;
	timekeep_g_time.tv_nsec = 0;
	itimer_init(&timekeep_g_itimer, kernel_clock_source, 0);

	return 0;
}
