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

#ifdef SMP /* XXX */
static struct sys_timer_sharing sched_tick_sharing;
#endif

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

#ifdef SMP /* XXX */
void sched_ticker_set_shared(void) {
	if (!timer_is_shared(&sched_tick_timer)) {
		sched_tick_timer.timer_sharing = &sched_tick_sharing;
		if(timer_is_started(&sched_tick_timer))
			sched_tick_timer.timer_sharing->shared_cpu = (0x1 << cpu_get_id());
		timer_set_shared(&sched_tick_timer);
	}else{
		panic("Once-called function, check up\n");
	}
}

void sched_ticker_set_private(void) {
	if (timer_is_shared(&sched_tick_timer)) {
		timer_set_private(&sched_tick_timer);
	}
}

void* sched_ticker_get_timer(void){
	return (void*)&sched_tick_timer;
}
#endif
