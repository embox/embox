/**
 * @file
 * @brief System time correction with NTP
 *
 * @date 25.07.2012
 * @author Alexander Kalmuk
 */

#include <kernel/time/clock_source.h>
#include <kernel/time/itimer.h>
#include <time.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(time_init);

static struct {
	struct clock_source *cs;
	int precision; /*< e.g. NTP precision */
	struct timespec time;
} abstime;

static struct itimer itimer;

static void time_set_clock_source(struct clock_source *cs) {
	abstime.cs = cs;
}

void settimeofday(struct timespec *newtime, struct timezone *tz) {
	abstime.time.tv_sec = newtime->tv_sec;
	abstime.time.tv_nsec = newtime->tv_nsec;
	itimer_init(&itimer, abstime.cs, 0);
}

void getnsofday(struct timespec *t, struct timezone *tz) {
	struct timespec ts = {
			.tv_sec = abstime.time.tv_sec,
			.tv_nsec = abstime.time.tv_nsec
	};
	ns_t cur = itimer.cs->read(itimer.cs);

	ts = timespec_add_ns(ts, cur);

	t->tv_sec = ts.tv_sec;
	t->tv_nsec = ts.tv_nsec;
}

static int time_init(void) {
	struct clock_source *cs;
	extern struct clock_source *kernel_clock_source;

	/* find clock_event_device with maximal resolution  */
	cs = kernel_clock_source;

	time_set_clock_source(cs);
	itimer_init(&itimer, cs, 0);

	return 0;
}
