/**
 * @file
 *
 * @brief
 *
 * @date 18.05.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>

#include <time.h>
#include <kernel/ktime.h>
#include <kernel/clock_event.h>
#include <kernel/time/timecounter.h>

EMBOX_UNIT_INIT(module_init);

static useconds_t time_usec(void) {
	return clock_source_clock_to_usec(clock_source_get_default(), clock());
}

struct ktimeval * get_timeval(struct ktimeval *tv) {
	useconds_t usec;

	usec = time_usec();
	tv->tv_sec = usec / USEC_PER_SEC;
	tv->tv_usec = usec % USEC_PER_SEC;
	return tv;
}

//TODO global time timecounter is bad
static struct timecounter sys_timecounter;

ns_t clock_get_systime(void) {
	return timecounter_read(&sys_timecounter);
}

static int module_init(void) {
	const struct clock_event_device *dev;

	/* find PIT timer */
	dev = cedev_get_by_name("pit");
	/* install timecounter value to 0 */
	timecounter_init(&sys_timecounter, dev->cs->cc, 0);
	return 0;
}
