/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_SYS_RESOURCE_H_
#define COMPAT_POSIX_SYS_RESOURCE_H_

#include <sys/types.h>
#include <sys/time.h>

#define RLIMIT_STACK 0
#define RLIMIT_CORE  1
#define RLIMIT_RTPRIO 2
#define RLIMIT_NOFILE 128
#define RLIM_INFINITY 0

#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2

#define RUSAGE_SELF 0

typedef unsigned int rlim_t;

struct rlimit {
	rlim_t rlim_cur; /* Soft limit */
	rlim_t rlim_max; /* Hard limit (ceiling for rlim_cur) */
};

struct rusage {
	struct timeval ru_utime; /* user CPU time used */
	struct timeval ru_stime; /* system CPU time used */
};

extern int getrlimit(int resource, struct rlimit *rlp);
extern int setrlimit(int resource, const struct rlimit *rlp);

extern int getpriority(int which, id_t who);
extern int setpriority(int which, id_t who, int value);

extern int getrusage(int who, struct rusage *usage);

#endif /* COMPAT_POSIX_SYS_RESOURCE_H_ */
