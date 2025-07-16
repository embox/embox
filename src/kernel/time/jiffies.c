/**
 * @file
 *
 * @brief Jiffies implementaion.
 * @details jiffies are generating by mostly precision clock source with event device.
 *
 * @date 10.04.2012
 * @author Anton Bondarev
 */

#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include <framework/mod/options.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#define CS_NAME OPTION_GET(STRING, cs_name)

extern struct clock_source CLOCK_SOURCE_NAME(CS_NAME);

const struct clock_source *cs_jiffies = &CLOCK_SOURCE_NAME(CS_NAME);

clock_t clock_sys_ticks(void) {
	if (!cs_jiffies) {
		return 0;
	}
	return (clock_t)cs_jiffies->event_device->jiffies;
}

clock_t ns2jiffies(time64_t ns) {
	assert(cs_jiffies->event_device);
	return ns_to_clock(cs_jiffies->event_device->event_hz, ns);
}

clock_t ms2jiffies(time64_t ms) {
	return ns2jiffies(ms * NSEC_PER_MSEC);
}

time64_t jiffies2ms(clock_t jiff) {
	time64_t ns;
	ns = clock_to_ns(cs_jiffies->event_device->event_hz, jiff);
	return ns / NSEC_PER_MSEC;
}

uint32_t clock_freq(void) {
	return cs_jiffies->event_device->event_hz;
}

int jiffies_init(void) {
	extern int clock_tick_init(void);

	clock_tick_init();

	clock_source_register((struct clock_source *)cs_jiffies);

	clock_source_set_periodic((struct clock_source *)cs_jiffies,
	    JIFFIES_PERIOD);

	return 0;
}
