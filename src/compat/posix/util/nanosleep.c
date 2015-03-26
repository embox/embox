/**
 * @file
 *
 * @date Sep 25, 2013
 * @author Anton Bondarev
 * @author Alexander Kalmuk
 */
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <util/math.h>

#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <embox/unit.h>

//#define NANOSLEEP_DEBUG

#ifdef NANOSLEEP_DEBUG
#include <kernel/printk.h>
#endif

EMBOX_UNIT_INIT(nanosleep_init);

static struct timespec nanosleep_waste_time;
static struct clock_source *nanosleep_cs;
static unsigned int nanosleep_cs_load;

static inline struct timespec get_timetosleep(const struct timespec *rqtp);
static void cs_nanospin(struct clock_source *cs, uint32_t cycles);
static inline uint32_t cs_nano_diff(struct clock_source *cs, uint32_t load, uint32_t old,
		uint32_t new);

/* XXX: nanosleep is irq sensetive, i.e. rqtp can be exceeded when irq occur
 * after ksleep and before returning from nanosleep. */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
	time64_t start, hw_cycles, tmp;
	uint32_t ms_tosleep;
	int remaining_cycles;
	struct timespec timetosleep;

	if (rqtp->tv_sec == 0 && rqtp->tv_nsec == 0) {
		return ksleep(0);
	}

	timetosleep = get_timetosleep(rqtp);
	hw_cycles = timespec_to_hw(&timetosleep, nanosleep_cs->counter_device->cycle_hz);

	start = clock_source_get_hwcycles(nanosleep_cs);
	ms_tosleep = timetosleep.tv_sec * 1000 + timetosleep.tv_nsec / 1000000;
	if (ms_tosleep > 0) {
		ksleep(ms_tosleep - 1);
	}

	remaining_cycles = (int64_t)hw_cycles - (clock_source_get_hwcycles(nanosleep_cs) - start);

	while (0 < remaining_cycles) {
		tmp = min(remaining_cycles, nanosleep_cs_load / 2);
		cs_nanospin(nanosleep_cs, tmp);
		remaining_cycles -= tmp;
	}

	return ENOERR;
}

void delay(int d) {
	//FIXME delay must plase in linux/delay.h

}

/* I use busy-wait for small amount of time (< 1 ms), because overhead on calculation is very small. */
static inline uint32_t cs_nano_diff(struct clock_source *cs, uint32_t load, uint32_t old,
		uint32_t new) {
	return new < old ? load - old + new : new - old;
}

static void cs_nanospin(struct clock_source *cs, uint32_t cycles) {
	uint32_t start = cs->counter_device->read();
	uint32_t load = nanosleep_cs_load - 1;
	while(cs_nano_diff(cs, load, start, cs->counter_device->read()) < cycles);
}

static inline struct timespec get_timetosleep(const struct timespec *rqtp) {
	struct timespec ret;

	if (rqtp->tv_sec > 0 || rqtp->tv_nsec > nanosleep_waste_time.tv_nsec) {
		ret = timespec_sub(*rqtp, nanosleep_waste_time);
	} else {
		ret = *rqtp;
	}

	return ret;
}

/**
 * Main logic:
 * 1. Calculate execution time of ktime_get_ns (i.e. overhead)
 * 2. Then we can calculate execution of nanosleep without going in ksleep and without spin (i.e. overhead).
 * 3. When we get T ns as argument in nanosleep, we substitute overhead calculated in step 2 from T.
 */
static int nanosleep_calibrate(void) {
	uint64_t t, m;
	const struct timespec rqtp = { .tv_sec = 0, .tv_nsec = 1 };

	m = ktime_get_ns();
	m = ktime_get_ns() - m;

#ifdef NANOSLEEP_DEBUG
	printk("ktime_get_ns execution ns: %d\n", (int) m);
#endif

	t = ktime_get_ns();
	nanosleep(&rqtp, NULL); // calculate nanosleep overhead
	t = ktime_get_ns() - t;
	nanosleep_waste_time.tv_nsec = t - m;

#ifdef NANOSLEEP_DEBUG
	printk("nanosleep execution ns: %d\n", (int) nanosleep_waste_time.tv_nsec);
#endif

	return 0;
}

static int nanosleep_init(void) {
	nanosleep_cs = clock_source_get_best(CS_WITH_IRQ);
	if (NULL == nanosleep_cs) {
		return -1;
	}
	nanosleep_cs_load =
			nanosleep_cs->counter_device->cycle_hz / nanosleep_cs->event_device->event_hz - 1;

	nanosleep_calibrate();

	return 0;
}
