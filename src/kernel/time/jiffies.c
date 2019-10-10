/**
 * @file
 *
 * @brief Jiffies implementaion.
 * @details jiffies are generating by mostly precision clock source with event device.
 *
 * @date 10.04.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

const struct clock_source *cs_jiffies;

clock_t clock_sys_ticks(void) {
	if (!cs_jiffies) {
		return 0;
	}
	return (clock_t)cs_jiffies->jiffies;
}

clock_t ns2jiffies(time64_t ns) {
	assert(cs_jiffies->event_device);
	return ns_to_clock(cs_jiffies->event_device->event_hz, ns);
}

clock_t ms2jiffies(time64_t ms) {
	return ns2jiffies(ms * 1000000);
}

time64_t jiffies2ms(clock_t jiff) {
	return clock_to_ns(cs_jiffies->event_device->event_hz, jiff) / 1000000;
}

uint32_t clock_freq(void) {
	return cs_jiffies->event_device->event_hz;
}

int jiffies_init(void) {
	const struct clock_source *cs;
	struct time_dev_conf jiffies_conf = {
		HW_TIMER_PERIOD
	};

	/* find clock_event_device with maximal frequency  */
	cs = clock_source_get_best(CS_WITH_IRQ);
	assert(cs);

	cs_jiffies = cs;

	/* set periodic mode */
	assert(cs->event_device->config);
	cs->event_device->config(&jiffies_conf);

	return 0;
}

int time_before(clock_t now, clock_t wait) {
       return now > wait;
}
