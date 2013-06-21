/**
 * @file
 *
 * @brief
 *
 * @date 21.06.2013
 * @author Anton Bondarev
 */

#include <kernel/thread.h>
#include <time.h>
/* FIXME: This operations is only for SMP */
void thread_set_affinity(struct thread *thread, unsigned int affinity) {
	thread->affinity = affinity;
}

unsigned int thread_get_affinity(struct thread *thread) {
	return thread->affinity;
}

/* FIXME: Replace it! */
struct thread *idle __percpu__;
clock_t cpu_started __percpu__;

void cpu_set_idle_thread(struct thread *thread) {
	thread->affinity = 1 << cpu_get_id();
	percpu_var(idle) = thread;
	percpu_var(cpu_started) = clock();
}

struct thread * cpu_get_idle_thread(void) {
	return percpu_var(idle);
}

clock_t cpu_get_total_time(unsigned int cpu_id) {
	return clock() - percpu_cpu_var(cpu_id, cpu_started);
}

clock_t cpu_get_idle_time(unsigned int cpu_id) {
	return thread_get_running_time(percpu_cpu_var(cpu_id, idle));
}
