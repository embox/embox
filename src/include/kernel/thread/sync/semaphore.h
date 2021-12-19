/**
 * @file
 * @brief Defines semaphore structure and methods associated with it.
 *
 * @date 02.09.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SYNC_SEMAPHORE_H_
#define KERNEL_THREAD_SYNC_SEMAPHORE_H_

#include <time.h>
#include <kernel/sched/waitq.h>

struct sem {
	struct waitq wq;
	int value;
	int max_value;
};

extern void semaphore_init(struct sem *s, int val);
extern void semaphore_enter(struct sem *s);
extern void semaphore_leave(struct sem *s);
extern int semaphore_tryenter(struct sem *s);
extern int semaphore_timedwait(struct sem *s, const struct timespec *abs_timeout);

extern int semaphore_getvalue(struct sem *s, int *sval);

/* Supports a completion use case
 */
#define semaphore_wait(s) semaphore_enter(s)
#define semaphore_trywait(s) semaphore_tryenter(s)
#define semaphore_post(s) semaphore_leave(s) 
void semaphore_post_all(struct sem *s);
void semaphore_reinit(struct sem *s);
extern int semaphore_timedwaitms(struct sem *s, long unsigned int timeout_ms);

#endif /* KERNEL_THREAD_SYNC_SEMAPHORE_H_ */
