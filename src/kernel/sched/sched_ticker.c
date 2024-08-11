/**
 * @file
 *
 * @date Jul 23, 2013
 * @author: Anton Bondarev
 */

#include <sched.h>

#include <hal/cpu.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include <kernel/time/timer.h>
#include <kernel/cpu/cpu.h>

#include <kernel/panic.h>

#include <framework/mod/options.h>

#define SCHED_TICK_INTERVAL \
	OPTION_GET(NUMBER, tick_interval)

static struct sys_timer sched_tick_timer[NCPU];

static void sched_tick(sys_timer_t *timer, void *param) {
	sched_post_switch();

#ifdef SMP
	for (int i = 0; i < NCPU; i++) {
		extern void smp_send_resched(int cpu_id);
		smp_send_resched(i);
	}
#endif /* SMP */
}

void sched_ticker_add(unsigned int cpuid) {
	if (!timer_is_started(&sched_tick_timer[cpuid])) {
		timer_init_start_msec(&sched_tick_timer[cpuid], TIMER_PERIODIC,
				SCHED_TICK_INTERVAL, sched_tick, NULL);
	}
}

void sched_ticker_del(unsigned int cpuid) {
	if (timer_is_started(&sched_tick_timer[cpuid])) {
		timer_stop(&sched_tick_timer[cpuid]);
	}
}
