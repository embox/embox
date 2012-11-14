/**
 * @file
 *
 * @brief Time subsystem based on timer's jiffies.
 *
 * @date 10.04.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>
#include <kernel/time/clock_source.h>

EMBOX_UNIT_INIT(module_init);

cycle_t volatile sys_ticks = 0; /* ticks after start system. */

clock_t clock_sys_ticks(void) {
	return (clock_t)sys_ticks;
}

static ns_t jiffies_cs_read(struct clock_source *cs) {
	return clock_sys_ticks();
}

struct clock_source jiffies = {
	.flags = 1,
	.read = jiffies_cs_read
};

clock_t ns_to_clock(__s64 ns) {
	return clock_source_ns_to_clock(&jiffies, ns);
}

ns_t clock_sys_ns(void) {
	return clock_source_clock_to_ns(&jiffies, clock_sys_ticks());
}

struct time_dev_conf jiffies_conf = {
	HW_TIMER_PERIOD
};

static int module_init(void) {
	const struct clock_source *cs;

	/* find clock_event_device with maximal resolution  */
	cs = clock_source_get_best(CS_WITH_IRQ);
	assert(cs);

	jiffies.event_device = cs->event_device;
	assert(jiffies.event_device);

	/* set periodic mode */
	cs->event_device->config(&jiffies_conf);

	return 0;
}
