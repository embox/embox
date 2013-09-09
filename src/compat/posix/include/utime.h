/**
 * @file
 * @brief
 *
 * @date 26.02.13
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_UTIME_H_
#define COMPAT_POSIX_UTIME_H_

#include <sys/types.h>

struct utimbuf {
	time_t actime;  /* access time */
	time_t modtime; /* modification time */
};

#endif /* COMPAT_POSIX_UTIME_H_ */
