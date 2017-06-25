/**
 * @file
 * @brief
 *
 * @date 22.07.13
 * @author Ilia Vaprol
 */

#include <kernel/cpu/cpu.h>
#include <kernel/thread.h>
#include <time.h>

clock_t cpu_get_total_time(unsigned int cpu_id) {
	return clock() - cpu_get_started(cpu_id);
}

clock_t cpu_get_idle_time(unsigned int cpu_id) {
	return thread_get_running_time(cpu_get_idle(cpu_id));
}

