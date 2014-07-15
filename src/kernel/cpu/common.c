/**
 * @file
 * @brief
 *
 * @date 19.07.13
 * @author Ilia Vaprol
 */

#include <module/embox/kernel/sched/strategy/affinity_api.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/cpudata.h>
#include <kernel/sched/affinity.h>
#include <kernel/thread.h>
#include <time.h>

static struct thread *idle __cpudata__ = NULL;
static clock_t started __cpudata__;

void cpu_init(unsigned int cpu_id, struct thread *idle_) {
	cpu_bind(cpu_id, idle_);
	cpudata_cpu_var(cpu_id, idle) = idle_;
	cpudata_cpu_var(cpu_id, started) = clock();
}

void cpu_bind(unsigned int cpu_id, struct thread *t) {
	//t->affinity = 1 << cpu_id;
	sched_affinity_set(&(t->runnable), 1 << cpu_id);
}

struct thread * cpu_get_idle(unsigned int cpu_id) {
	return cpudata_cpu_var(cpu_id, idle);
}

clock_t cpu_get_started(unsigned int cpu_id) {
	return cpudata_cpu_var(cpu_id, started);
}
