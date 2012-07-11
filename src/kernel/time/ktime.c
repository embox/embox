/**
 * @file
 *
 * @brief
 *
 * @date 18.05.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>

#include <kernel/time/itimer.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>

EMBOX_UNIT_INIT(module_init);

static struct itimer sys_timecounter;

ns_t ktime_get_ns(void) {
	return itimer_read(&sys_timecounter);
}

struct timeval *ktime_get_timeval(struct timeval *tv) {
	ns_t ns;

	ns = ktime_get_ns();
	tv->tv_sec = ns / NSEC_PER_SEC;
	tv->tv_usec = (ns % NSEC_PER_SEC) / NSEC_PER_USEC;
	return tv;
}

static int module_init(void) {
	const struct clock_source *cs;

	/* find clock_event_device with maximal resolution  */
	cs = clock_source_get_best(CS_ANY);
	assert(cs);

	itimer_init(&sys_timecounter, cs, 0);

	return 0;
}
