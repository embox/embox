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
#include <util/log.h>

EMBOX_UNIT_INIT(nanosleep_init);

struct hw_time {
	uint32_t jiffies;
	uint32_t cycles;
};

static struct timespec nanosleep_waste_time;
static struct clock_source *nanosleep_cs;
static unsigned int nanosleep_cs_load;

static inline struct timespec get_timetosleep(const struct timespec *rqtp);
static void cs_nanospin(struct clock_source *cs, struct hw_time *hw);

/* XXX: nanosleep is irq sensetive, i.e. rqtp can be exceeded when irq occur
 * after ksleep and before returning from nanosleep. */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
	time64_t start, hw_cycles;
	uint32_t ms_tosleep;
	int remaining_cycles;
	struct timespec timetosleep;
	struct hw_time hw;

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

	if (remaining_cycles > 0) {
		int tmp;

		tmp = remaining_cycles + nanosleep_cs->counter_device->read();
		hw.jiffies = nanosleep_cs->jiffies + tmp / nanosleep_cs_load;
		hw.cycles = tmp % nanosleep_cs_load;

		cs_nanospin(nanosleep_cs, &hw);
	}

	return ENOERR;
}

void delay(int d) {
	//FIXME delay must plase in linux/delay.h

}

static void cs_nanospin(struct clock_source *cs, struct hw_time *hw) {
	while(cs->jiffies < hw->jiffies);
	while((cs->jiffies == hw->jiffies) && (cs->counter_device->read() <= hw->cycles));
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

	log_info("ktime_get_ns execution ns: %d", (int) m);

	t = ktime_get_ns();
	nanosleep(&rqtp, NULL); // calculate nanosleep overhead
	t = ktime_get_ns() - t;
	nanosleep_waste_time.tv_nsec = t - m;

	log_info("nanosleep execution ns: %d", (int) nanosleep_waste_time.tv_nsec);

	return 0;
}

static int nanosleep_init(void) {
	nanosleep_cs = clock_source_get_best(CS_WITH_IRQ);
	if (NULL == nanosleep_cs) {
		return -1;
	}
	nanosleep_cs_load =
			nanosleep_cs->counter_device->cycle_hz / nanosleep_cs->event_device->event_hz;

	nanosleep_calibrate();

	return 0;
}
