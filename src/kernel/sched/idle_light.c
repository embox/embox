#include <util/err.h>
#include <hal/arch.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>

static struct lthread idle;

static int idle_run(struct lthread *self) {
	arch_idle();
	lthread_launch(self);
	return 0;
}

int idle_thread_create(void) {

	lthread_init(&idle, idle_run);

	sched_change_priority(&idle.schedee, SCHED_PRIORITY_MIN);
#if 0 //XX
	cpu_init(cpu_get_id(), t);
#else
	sched_affinity_set(&idle.schedee.affinity, 1 << cpu_get_id());
#endif
	lthread_launch(&idle);

	return 0;
}
