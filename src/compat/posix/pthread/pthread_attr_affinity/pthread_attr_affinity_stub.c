/**
 * @file   
 * @brief
 * @date   25.04.2025
 * @author Anton Bondarev
 */

#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

int pthread_attr_setaffinity_np(pthread_attr_t *attr, size_t cpusetsize,
	const cpu_set_t *cpuset) {
	return 0;
}

int pthread_attr_getaffinity_np(const pthread_attr_t *attr,	size_t cpusetsize,
	cpu_set_t *cpuset) {
	return 0;
}
