/**
 * @file   
 * @brief
 * @date   25.04.2025
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
							const cpu_set_t *cpuset) {
	return 0;
}

int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize,
							cpu_set_t *cpuset) {
	return 0;
}