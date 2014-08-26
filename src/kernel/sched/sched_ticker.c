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
#include <kernel/sched/schedee.h>

#include <kernel/time/timer.h>
#include <kernel/cpu/cpu.h>

#include <kernel/panic.h>

#include <framework/mod/options.h>

#define SCHED_TICK_INTERVAL \
	OPTION_GET(NUMBER, tick_interval)

static struct sys_timer *sched_tick_timer;

static void sched_tick(sys_timer_t *timer, void *param) {
	sched_post_switch();

#ifdef SMP
	for (int i = 0; i < NCPU; i++) {
		extern void smp_send_resched(int cpu_id);
		smp_send_resched(i);
	}
#endif /* SMP */
}

void sched_ticker_init(void) {
	if (timer_set(&sched_tick_timer, TIMER_PERIODIC, SCHED_TICK_INTERVAL,
			sched_tick, NULL)) {
		panic("Scheduler initialization failed!\n");
	}
}

void sched_ticker_fini(void) {
	timer_close(sched_tick_timer);  // TODO err check?
}

void sched_ticker_switch(struct schedee *prev, struct schedee *next) {
	if (prev->policy == SCHED_FIFO &&
		next->policy != SCHED_FIFO) {
		sched_ticker_init();
	}

	if (prev->policy != SCHED_FIFO &&
		next->policy == SCHED_FIFO) {
		sched_ticker_fini();
	}
}

