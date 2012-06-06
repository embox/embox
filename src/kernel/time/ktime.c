/**
 * @file
 *
 * @brief
 *
 * @date 18.05.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>

#include <kernel/clock_event.h>
#include <kernel/time/timecounter.h>

#include <kernel/ktime.h>

EMBOX_UNIT_INIT(module_init);

static struct timecounter sys_timecounter;

ns_t ktime_get_ns(void) {
	return timecounter_read(&sys_timecounter);
}

struct ktimeval * ktime_get_timeval(struct ktimeval *tv) {
	ns_t ns;

	ns = ktime_get_ns();
	tv->tv_sec = ns / NSEC_PER_SEC;
	tv->tv_usec = (ns % NSEC_PER_SEC) / NSEC_PER_USEC;
	return tv;
}

static int module_init(void) {
	const struct clock_event_device *dev;

	/* find clock_event_device with maximal resolution  */
	dev = cedev_get_best(TICKS);
	assert(dev);

	/* install timecounter value to 0 */
	dev->init();
	assert(dev->cs);
	timecounter_init(&sys_timecounter, dev->cs, 0);

	return 0;
}

