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
/* TODO SCHED_FIFO and SCHED_RR may have priority more or equal 200 */
#define SCHED_FIFO      1 /* First in-first out (FIFO) scheduling policy*/
#define SCHED_RR        2 /* Round robin scheduling policy. */


struct sched_param {
	int sched_priority; /**< Process or thread execution scheduling priority. */
	/*
	In addition, if _POSIX_SPORADIC_SERVER or _POSIX_THREAD_SPORADIC_SERVER is
	defined, the sched_param structure defined in <sched.h> shall contain the
	following members in addition to those specified above:

	int             sched_ss_low_priority Low scheduling priority for
	                                      sporadic server.
	struct timespec sched_ss_repl_period  Replenishment period for
	                                      sporadic server.
	struct timespec sched_ss_init_budget  Initial budget for sporadic server.
	int             sched_ss_max_repl     Maximum pending replenishments for
	                                      sporadic server.
	*/
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


/* _GNU_SOURCE */
#include <kernel/sched/affinity.h>

typedef struct affinity cpu_set_t;

extern void CPU_ZERO(cpu_set_t *set);

extern void CPU_SET(int cpu, cpu_set_t *set);
extern void CPU_CLR(int cpu, cpu_set_t *set);
extern int  CPU_ISSET(int cpu, cpu_set_t *set);

extern int sched_getcpu(void);


__END_DECLS

#endif /* POSIX_SCHED_H_ */
