/**
 * @file
 * @brief
 *
 * @date   27.03.2013
 * @author Anton Bulychev
 */

#include <errno.h>
#include <pthread.h>

#include <kernel/thread.h>
#include <kernel/thread/types.h>
#include <kernel/thread/thread_stack.h>
#include <util/err.h>

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize) {
	return -ENOSYS;
}