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

EMBOX_UNIT_INIT(module_init);

const struct clock_source *cs_jiffies;

clock_t clock_sys_ticks(void) {
	return (clock_t)cs_jiffies->jiffies;
}

clock_t ns2jiffies(ns_t ns) {
	return clock_source_ns_to_clock(( struct clock_source *)cs_jiffies, ns);
}

static int module_init(void) {
	const struct clock_source *cs;
	struct time_dev_conf jiffies_conf = {
		HW_TIMER_PERIOD
	};

	/* find clock_event_device with maximal resolution  */
	cs = clock_source_get_best(CS_WITH_IRQ);
	assert(cs);

	cs_jiffies = cs;

	/* set periodic mode */
	assert(cs->event_device->config);
	cs->event_device->config(&jiffies_conf);

	return 0;
}
