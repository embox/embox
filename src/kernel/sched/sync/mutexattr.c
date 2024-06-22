/**
 * @file
 * @brief
 *
 * @date   24.09.2013
 * @author Vita Loginova
 */

#include <errno.h>
#include <stddef.h>

#include <kernel/sched/sync/mutexattr.h>

int mutexattr_init(struct mutexattr *attr) {
	attr->type = MUTEX_DEFAULT;

	return ENOERR;
}

int mutexattr_copy(const struct mutexattr *source_attr,
    struct mutexattr *dest_attr) {
	dest_attr->type = source_attr->type;

	return ENOERR;
}

int mutexattr_destroy(struct mutexattr *attr) {
	attr->type = 0;

	return ENOERR;
}

int mutexattr_gettype(const struct mutexattr *attr, int *type) {
	*type = attr->type;

	return ENOERR;
}

int mutexattr_settype(struct mutexattr *attr, int type) {
	attr->type = type;

	return ENOERR;
}
