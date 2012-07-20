/*
 * @brief
 *
 * @date 17.07.2012
 * @author Alexander Kalmuk
 */

#include <kernel/time/clock_source.h>
#include <kernel/time/itimer.h>
#include <kernel/time/time.h>
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

static void set_normalized_timespec(struct timespec *ts,
		time_t sec, long nsec) {
	while (nsec >= NSEC_PER_SEC) {
		nsec -= NSEC_PER_SEC;
		sec++;
	}

	while (nsec < 0) {
		nsec += NSEC_PER_SEC;
		sec--;
	}

	ts->tv_sec = sec;
	ts->tv_nsec = nsec;
}

struct timespec timespec_add(struct timespec t1,
		struct timespec t2) {
	struct timespec ts;

	set_normalized_timespec(&ts, t1.tv_sec + t2.tv_sec,
			t1.tv_nsec + t2.tv_nsec);

	return ts;
}

struct timespec timespec_sub(struct timespec t1,
		struct timespec t2) {
	struct timespec ts;

	set_normalized_timespec(&ts, t1.tv_sec - t2.tv_sec,
			t1.tv_nsec - t2.tv_nsec);

	return ts;
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

