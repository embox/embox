#ifndef SQLITE_SYS_TIME_H_
#define SQLITE_SYS_TIME_H_

#include_next <sys/time.h>

extern int utimes(const char *path, const struct timeval times[2]);

#endif /* SQLITE_SYS_TIME_H_ */
