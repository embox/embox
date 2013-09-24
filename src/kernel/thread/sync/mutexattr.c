/**
 * @file
 * @brief
 *
 * @date   24.09.2013
 * @author Vita Loginova
 */

#include <errno.h>
#include <defines/null.h>
#include <kernel/thread/sync/mutexattr.h>

int mutexattr_init(struct mutexattr *attr) {
	attr->type = PTHREAD_MUTEX_DEFAULT;

	return ENOERR;
}

int mutexattr_destroy(struct mutexattr *attr) {
	attr->type = 0;

	return ENOERR;
}
