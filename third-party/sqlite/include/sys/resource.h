#ifndef SQLITE_SYS_RESOURCE_H_
#define SQLITE_SYS_RESOURCE_H_

#include <sys/time.h>

#define RUSAGE_SELF 0

struct rusage {
    struct timeval ru_utime; /* user CPU time used */
    struct timeval ru_stime; /* system CPU time used */
};

extern int getrusage(int who, struct rusage *r_usage);

#include_next <sys/resource.h>

#endif /* SQLITE_SYS_RESOURCE_H_ */
