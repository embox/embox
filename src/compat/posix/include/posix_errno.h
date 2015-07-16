/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.06.2012
 */

#ifndef COMPAT_POSIX_POSIX_ERRNO_H_
#define COMPAT_POSIX_POSIX_ERRNO_H_

#include <kernel/task/resource/errno.h>
#include <compiler.h>

#define errno (*task_self_resource_errno())

static inline int SET_ERRNO(int err) {
	errno = err;
	return -1;
}

#endif /* COMPAT_POSIX_POSIX_ERRNO_H_ */
