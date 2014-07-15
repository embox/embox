#ifndef SQLITE_UNISTD_H_
#define SQLITE_UNISTD_H_

#include <sys/types.h>

extern int fchown(int fildes, uid_t owner, gid_t group);

#include_next <unistd.h>

#endif /* SQLITE_UNISTD_H_ */
