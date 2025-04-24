/**
 * @file   
 * @brief  pthread_barrier functions
 * @date   11.03.2020
 * @author Sobhan Mondal <inro20fdm@gmail.com>
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