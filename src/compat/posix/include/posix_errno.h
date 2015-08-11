/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.06.2012
 */

#ifndef COMPAT_POSIX_POSIX_ERRNO_H_
#define COMPAT_POSIX_POSIX_ERRNO_H_

#include <module/embox/compat/posix/errno/api.h>
#include <compiler.h>

static inline int SET_ERRNO(int err) {
	errno = err;
	return -1;
}

#endif /* COMPAT_POSIX_POSIX_ERRNO_H_ */
