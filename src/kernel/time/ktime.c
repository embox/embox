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

//TODO global time timecounter is bad
static struct timecounter sys_timecounter;

ns_t clock_get_systime(void) {
	return timecounter_read(&sys_timecounter);
}

static int module_init(void) {
	const struct clock_event_device *dev;

	/* find clock_event_device with maximal resolution  */
	dev = cedev_get_best(TICKS);
	assert(dev);

	/* install timecounter value to 0 */
	dev->init();
	timecounter_init(&sys_timecounter, dev, 0);

	return 0;
}

//TODO ktime has bad function
struct ktimeval * get_timeval(struct ktimeval *tv) {
	ns_t ns;

	ns = timecounter_read(&sys_timecounter);
	tv->tv_sec = ns / NSEC_PER_SEC;
	tv->tv_usec = (ns % NSEC_PER_SEC) / 1000;
	return tv;
}
