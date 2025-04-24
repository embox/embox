/*
 * sched_posix.c
 *
 *  Created on: 5 feb. 2013
 *      Author: fsulima
 */

#include <sched.h>

#include <kernel/sched.h>

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