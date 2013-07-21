/**
 * @file
 * @brief
 *
 * @date 19.07.13
 * @author Ilia Vaprol
 */

#include <kernel/cpu.h>
#include <kernel/thread.h>
#include <time.h>

static struct thread *cpu_idle __cpudata__;
static clock_t cpu_started __cpudata__;

void cpu_set_idle_thread(struct thread *idle) {
	thread_set_affinity(idle, 1 << cpu_get_id());
	cpudata_var(cpu_idle) = idle;
	cpudata_var(cpu_started) = clock();
}

struct thread * cpu_get_idle_thread(void) {
	return cpudata_var(cpu_idle);
}

clock_t cpu_get_total_time(unsigned int cpu_id) {
	return clock() - cpudata_cpu_var(cpu_id, cpu_started);
}

clock_t cpu_get_idle_time(unsigned int cpu_id) {
	return thread_get_running_time(cpudata_cpu_var(cpu_id, cpu_idle));
}
