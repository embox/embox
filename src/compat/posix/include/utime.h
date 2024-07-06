/**
 * @file
 * @brief
 *
 * @date 26.02.13
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_UTIME_H_
#define COMPAT_POSIX_UTIME_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

struct utimbuf {
	time_t actime;  /* access time */
	time_t modtime; /* modification time */
};

extern int utime(const char *path, const struct utimbuf *times);

__END_DECLS

#endif /* COMPAT_POSIX_UTIME_H_ */
