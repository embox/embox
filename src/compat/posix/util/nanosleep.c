/**
 * @file
 *
 * @date Sep 25, 2013
 * @author: Anton Bondarev
 * @author: Alexander Kalmuk
 */
#include <errno.h>
#include <time.h>
#include <stdlib.h>

#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <embox/unit.h>

#define NANOSLEEP_DEBUG

#ifdef NANOSLEEP_DEBUG
#include <kernel/printk.h>
#endif

EMBOX_UNIT_INIT(nanosleep_calibrate);

static int nanosleep_exec_time;

/* I use busy-wait for small amount of time (< 1 ms), because overhead on calculation is very small. */
static uint32_t cs_nano_diff(struct clock_source *cs, uint32_t old, uint32_t new) {
	uint32_t load = cs->counter_device->cycle_hz / cs->event_device->event_hz - 1;
	uint32_t ticks = new < old ? load - old + new : new - old;
	return (((uint64_t) ticks * cs->counter_mult) >> cs->counter_shift);
}

static void cs_nanospin(struct clock_source *cs, uint32_t ns) {
	uint32_t start = cs->counter_device->read();
	while(cs_nano_diff(cs, start, cs->counter_device->read()) < ns);
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

	/* calculate ktime_get_ns overhead  */
	t = ktime_get_ns();
	t = ktime_get_ns() - t;

#ifdef NANOSLEEP_DEBUG
	printk("ktime_get_ns execution ns: %d\n", (int)t);
#endif

	m = ktime_get_ns();
	nanosleep(&rqtp, NULL); // calculate nanosleep overhead
	m = ktime_get_ns() - m;
	nanosleep_exec_time = m - t;

#ifdef NANOSLEEP_DEBUG
	printk("nanosleep execution ns: %d\n", nanosleep_exec_time);
#endif

	return 0;
}

/* XXX: nanosleep is irq sensetive, i.e. rqtp can be exceeded when irq occur
 * after ksleep and before returning from nanosleep. */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
	time64_t rqtp_ns, start, tmp;
	uint32_t ms;
	int remaining_time;
	struct clock_source *cs;

	start = ktime_get_ns();

	cs = clock_source_get_best(CS_WITH_IRQ);
	rqtp_ns = timespec_to_ns(rqtp);

	if (rqtp_ns < nanosleep_exec_time) {
		return ENOERR;
	}
	rqtp_ns -= nanosleep_exec_time;

	ms = rqtp->tv_sec * 1000 + rqtp->tv_nsec / 1000000;
	if (ms > 0) {
		ksleep(ms);
	}

	tmp = ktime_get_ns();
	/* remaining time < 2 ms */
	remaining_time = rqtp_ns - (tmp - start);
	if (remaining_time > 0) {
		/* Split in two steps, because of cs_nanospins works only for times < 1 ms */
		cs_nanospin(cs, remaining_time / 2); // < 1 ms
		cs_nanospin(cs, remaining_time / 2); // < 1 ms
	}

	return ENOERR;
}

void delay(int d) {
	//FIXME delay must plase in linux/delay.h

}
