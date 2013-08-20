/**
 * @file
 *
 * @date Jul 23, 2013
 * @author: Anton Bondarev
 */

#include <framework/mod/options.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include <kernel/time/timer.h>
#include <kernel/cpu/cpu.h>
#include <module/embox/arch/smp.h>

#include <kernel/panic.h>


static struct sys_timer *sched_tick_timer;

#define SCHED_TICK    OPTION_GET(NUMBER, tick_interval)

static void sched_tick(sys_timer_t *timer, void *param) {
	extern void smp_send_resched(int cpu_id);
	sched_post_switch();

#ifndef NOSMP
	for (int i = 0; i < NCPU; i++) {
		smp_send_resched(i);
	}
#endif /* !NOSMP */
}


int sched_ticker_init(void) {
	/* Initializing tick timer. */
	if (timer_set(&sched_tick_timer, TIMER_PERIODIC, SCHED_TICK, sched_tick, NULL)) {
		panic("Scheduler initialization failed!\n");
	}
	return 0;
}

void sched_ticker_fini(struct runq *rq) {
	timer_close(sched_tick_timer);
}

