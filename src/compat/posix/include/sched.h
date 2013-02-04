/**
 * @file
 *
 * @brief POSIX (REALTIME) execution scheduling
 *
 * @details http://pubs.opengroup.org/onlinepubs/7908799/xsh/sched.h.html
 *
 * @date Jan 17, 2013
 * @author: Anton Bondarev
 */

#ifndef POSIX_SCHED_H_
#define POSIX_SCHED_H_

#include <sys/types.h>
#include <time.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

/*
 * Scheduling policies
 */
#define SCHED_OTHER     0 /* Another scheduling policy.*/
#define SCHED_FIFO      1 /* First in-first out (FIFO) scheduling policy*/
#define SCHED_RR        2 /* Round robin scheduling policy. */


struct sched_param {
	int sched_priority; /**<  process execution scheduling priority */
};

/**
 * sched_get_priority_max, get max priority  limit
 */
extern int sched_get_priority_max(int);

/**
 * sched_get_priority_min - get min priority limits
 */
extern int sched_get_priority_min(int);

/**
 * get scheduling parameters
 */
extern int sched_getparam(pid_t, struct sched_param *);

/**
 * get scheduling policy
 */
extern int sched_getscheduler(pid_t);

/**
 * get execution time limits
 */
extern int sched_rr_get_interval(pid_t, struct timespec *);

/**
 * set scheduling parameters
 */
extern int sched_setparam(pid_t, const struct sched_param *);

/**
 * set scheduling policy and parameters
 */
extern int sched_setscheduler(pid_t, int, const struct sched_param *);

/**
 *  yield processor
 */
extern int sched_yield(void);

__END_DECLS

#endif /* POSIX_SCHED_H_ */
