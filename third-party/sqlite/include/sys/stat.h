#ifndef SQLITE_SYS_STAT_H_
#define SQLITE_SYS_STAT_H_

#include <sys/types.h>

extern int fchmod(int fildes, mode_t mode);

#include_next <sys/stat.h>

#endif /* SQLITE_SYS_STAT_H_ */
