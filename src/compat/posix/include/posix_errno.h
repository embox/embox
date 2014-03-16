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

#define errno (*task_self_resource_errno())

#define SET_ERRNO(e) \
	({ errno = e; -1; /* to let 'return SET_ERRNO(...)' */ })

#endif /* COMPAT_POSIX_POSIX_ERRNO_H_ */
