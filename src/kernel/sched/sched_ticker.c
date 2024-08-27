/**
 * @file
 *
 * @date Jul 23, 2013
 * @author: Anton Bondarev
 */

#include <sched.h>

#include <hal/cpu.h>
#include <kernel/sched.h>

#include <kernel/time/timer.h>
#include <kernel/cpu/cpu.h>

#include <kernel/panic.h>

#include <framework/mod/options.h>

#define SCHED_TICK_INTERVAL \
	OPTION_GET(NUMBER, tick_interval)

static struct sys_timer sched_tick_timer;

static void sched_tick(sys_timer_t *timer, void *param) {
#ifdef SMP
	for (int i = 0; i < NCPU; i++) {
		extern void smp_send_resched(int cpu_id);
		if(i == cpu_get_id()) {
			continue;
		}
		smp_send_resched(i);
	}
#endif /* SMP */
	sched_post_switch();
}

void sched_ticker_add(void) {
	if (!timer_is_started(&sched_tick_timer)) {
		timer_init_start_msec(&sched_tick_timer, TIMER_PERIODIC,
				SCHED_TICK_INTERVAL, sched_tick, NULL);
	}
}

void sched_ticker_del(void) {
	if (timer_is_started(&sched_tick_timer)) {
		timer_stop(&sched_tick_timer);
	}
}
