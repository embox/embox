/*
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

struct __time {
	struct clock_source *cs;
	int precision; /*< e.g. NTP precision */
	struct timespec time;
};

static struct __time abstime;
static struct itimer itimer;

static void time_set_clock_source(struct clock_source *cs) {
	abstime.cs = cs;
}

void settimeofday(struct timespec *newtime, struct timezone *tz) {
	abstime.time.tv_sec = newtime->tv_sec;
	abstime.time.tv_nsec = newtime->tv_nsec;
	itimer_init(&itimer, abstime.cs, 0);
}

void gettimeofday(struct timespec *t, struct timezone *tz) {
	ns_t cur = itimer.cs->read(itimer.cs);

	t->tv_sec = abstime.time.tv_sec + cur / NSEC_PER_SEC;
	t->tv_nsec = abstime.time.tv_nsec + cur % NSEC_PER_SEC;
}

static int time_init(void) {
	struct clock_source *cs;

	/* find clock_event_device with maximal resolution  */
	cs = clock_source_get_best(CS_ANY);
	assert(cs);

	time_set_clock_source(cs);
	itimer_init(&itimer, cs, 0);

	return 0;
}
