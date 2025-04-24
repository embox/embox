/**
 * @file
 *
 * @date 5 feb. 2013
 * @author Anton Bondarev
 */

#include <sched.h>

#include <kernel/sched.h>

#include <hal/cpu.h>

void CPU_ZERO(cpu_set_t *set) {
	sched_affinity_init(set);
}

void CPU_SET(int cpu, cpu_set_t *set) {
	sched_affinity_set(set, 1 << cpu);
}

void CPU_CLR(int cpu, cpu_set_t *set) {

}

int  CPU_ISSET(int cpu, cpu_set_t *set) {
	return sched_affinity_check(set, 1 << cpu);
}

int sched_getcpu(void) {
	uint32_t cpuid = cpu_get_id();
	return cpuid;
}