/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2014
 */

#include <stdint.h>
#include <sys/types.h>

#include <hal/clock.h>
#include <hal/cpu_idle.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <kernel/time/sys_timer.h>

void arch_cpu_idle(void) {
	extern const struct clock_source *cs_jiffies;

	struct clock_source *cs;
	uint64_t next_event_cycles;
	clock_t next_event_ticks;
	clock_t sleep_ticks;

	if (!cs_jiffies) {
		return;
	}

	cs = (struct clock_source *)cs_jiffies;

	if (timer_strat_get_next_event(&next_event_ticks) != 0) {
		/* Sleep as long as possible */
		next_event_ticks = UINT32_MAX;
	}
	else {
		next_event_ticks = next_event_ticks - clock_sys_ticks();
	}

	next_event_cycles = clock_source_ticks2cycles(cs, next_event_ticks);
	if (next_event_cycles > cs->counter_device->mask) {
		next_event_cycles = cs->counter_device->mask;
	}

	clock_source_set_oneshot(cs);
	clock_source_set_next_event(cs, next_event_cycles);

	__asm__ __volatile__("WFI");

	sleep_ticks = clock_source_cycles2ticks(cs, clock_source_get_cycles(cs));

	jiffies_update(sleep_ticks);

	clock_source_set_periodic(cs, cs->event_device->event_hz);
}
