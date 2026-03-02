/**
 * @file
 *
 * @date Sep 25, 2013
 * @author Anton Bondarev
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <embox/unit.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <util/log.h>
#include <util/math.h>

#include "kernel/time/time.h"
#include "sys/time.h"

EMBOX_UNIT_INIT(nanosleep_init);

#if 0
struct hw_time {
	uint32_t jiffies;
	uint32_t cycles;
};

static struct timespec nanosleep_waste_time;
static struct clock_source *nanosleep_cs;
static unsigned int nanosleep_cs_load;
#endif

// static inline struct timespec get_timetosleep(const struct timespec *rqtp);
// static void cs_nanospin(struct clock_source *cs, struct hw_time *hw);

/* XXX: nanosleep is irq sensetive, i.e. rqtp can be exceeded when irq occur
 * after ksleep and before returning from nanosleep. */
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
    struct timespec *rmtp) {
	uint32_t ms_tosleep;
	struct timespec timetosleep, timetowake, now;

	if (clock_id != CLOCK_MONOTONIC && clock_id != CLOCK_REALTIME) {
		return EINVAL;
	}

	if (rqtp->tv_sec < 0 || rqtp->tv_nsec < 0
	    || rqtp->tv_nsec >= (long)NSEC_PER_SEC) {
		return EINVAL;
	}

	ktime_get_timespec(&now);

	if ((flags & TIMER_ABSTIME) != 0) {
		timetosleep = timespec_sub(*rqtp, now);
		timetowake = *rqtp;
	}
	else {
		timetosleep = *rqtp;
		timetowake = timespec_add(now, *rqtp);
	}

	if ( ( timetosleep.tv_sec < 0 ) ||
			(timetosleep.tv_sec == 0 && timetosleep.tv_nsec == 0)) {
		return ksleep(0);
	}

	//timetosleep = get_timetosleep(&timetosleep);
	// ms_tosleep = timetosleep.tv_sec * 1000 + timetosleep.tv_nsec / 1000000;
	ms_tosleep = timespec_to_ms(&timetosleep);
	if (ms_tosleep > 0) {
		ksleep(ms_tosleep - 1);
	}

	// now.tv_sec = 0;
	// now.tv_nsec = 1000;
	// timetowake =  timespec_add(timetowake, now);

	do {
		ktime_get_timespec(&now);
	} while ( (now.tv_sec < timetowake.tv_sec) ||
		(now.tv_sec == timetowake.tv_sec && now.tv_nsec < timetowake.tv_nsec) );

	if (rmtp != NULL) {
		rmtp->tv_nsec = 0;
		rmtp->tv_sec = 0;
	}

	return ENOERR;
}

/* XXX: nanosleep is irq sensetive, i.e. rqtp can be exceeded when irq occur
 * after ksleep and before returning from nanosleep. */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
	return clock_nanosleep(CLOCK_REALTIME, 0, rqtp, rmtp);
}

// static void cs_nanospin(struct clock_source *cs, struct hw_time *hw) {
// 	while (cs->event_device->jiffies < hw->jiffies)
// 		;
// 	while ((cs->event_device->jiffies == hw->jiffies)
// 	       && (cs->counter_device->get_cycles(cs) <= hw->cycles))
// 		;
// }

// static inline struct timespec get_timetosleep(const struct timespec *rqtp) {
// 	struct timespec ret;

// 	if (rqtp->tv_sec > 0 || rqtp->tv_nsec > nanosleep_waste_time.tv_nsec) {
// 		ret = timespec_sub(*rqtp, nanosleep_waste_time);
// 	}
// 	else {
// 		ret = *rqtp;
// 	}

// 	return ret;
// }

#if 0
/**
 * Main logic:
 * 1. Calculate execution time of ktime_get_timespec (i.e. overhead)
 * 2. Then we can calculate execution of nanosleep without going in ksleep and without spin (i.e. overhead).
 * 3. When we get T ns as argument in nanosleep, we substitute overhead calculated in step 2 from T.
 */
static int nanosleep_calibrate(void) {
	struct timespec t, m, ts;
	struct timespec rqtp = {.tv_sec = 0, .tv_nsec = 1};

	ktime_get_timespec(&t);
	ktime_get_timespec(&m);
	timespecsub(&m, &t, &ts);

	log_info("ktime_get_timespec() execution ns: %d", (int)ts.tv_nsec);

	ktime_get_timespec(&t);
	clock_nanosleep(CLOCK_MONOTONIC, 0, &rqtp, NULL);
	ktime_get_timespec(&m);
	timespecsub(&m, &t, &rqtp);

	timespecsub(&rqtp, &ts, &nanosleep_waste_time);

	log_info("nanosleep execution ns: %d", (int)nanosleep_waste_time.tv_nsec);

	return 0;
}
#endif

static int nanosleep_init(void) {
#if 0
	nanosleep_cs = clock_source_get_best(CS_WITH_IRQ);
	if (NULL == nanosleep_cs) {
		return -1;
	}
	nanosleep_cs_load = nanosleep_cs->counter_device->cycle_hz
	                    / nanosleep_cs->event_device->event_hz;

	nanosleep_calibrate();
#endif
	return 0;
}
