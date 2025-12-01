/**
 * @file
 * @brief
 *
 * @date   27.03.2013
 * @author Anton Bulychev
 */

#ifndef PTHREAD_H_
#define PTHREAD_H_

/*The following types shall be defined as described in <sys/types.h> :
 * ...
 */
#include <sys/types.h>

/* Inclusion of the <sched.h> header may make visible all symbols from
 * the <time.h> header.
 */
#include <sched.h>
#include <semaphore.h>

#include <kernel/thread/thread_flags.h>
#include <kernel/thread/sync/cond.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/sched/sync/mutexattr.h>
#include <kernel/thread/sync/rwlock.h>
#include <kernel/task/thread_key_table.h>

#define PTHREAD_MUTEX_INITIALIZER   MUTEX_INIT_STATIC
#define PTHREAD_RMUTEX_INITIALIZER  RMUTEX_INIT_STATIC

#define PTHREAD_MUTEX_NORMAL        MUTEX_NORMAL
#define PTHREAD_MUTEX_ERRORCHECK    MUTEX_ERRORCHECK
#define PTHREAD_MUTEX_RECURSIVE     MUTEX_RECURSIVE
#define PTHREAD_MUTEX_DEFAULT       MUTEX_DEFAULT

#define PTHREAD_PROCESS_SHARED      PROCESS_SHARED
#define PTHREAD_PROCESS_PRIVATE     PROCESS_PRIVATE

#define PTHREAD_SCOPE_PROCESS       THREAD_SCOPE_PROCESS
#define PTHREAD_SCOPE_SYSTEM        THREAD_SCOPE_SYSTEM



struct thread;
typedef struct thread *pthread_t;


#define PTHREAD_INHERIT_SCHED       THREAD_FLAG_PRIORITY_INHERIT
#define PTHREAD_EXPLICIT_SCHED      THREAD_FLAG_PRIORITY_EXPLICIT

#define PTHREAD_CREATE_DETACHED     THREAD_FLAG_DETACHED
#define PTHREAD_CREATE_JOINABLE     THREAD_FLAG_JOINABLE

#define PTHREAD_BARRIER_SERIAL_THREAD (-1)

#if 0
typedef struct pthread_attr {
	uint32_t flags; /* scope, inherit, detachstate */
	void *stack;
	uint32_t stack_size;
	int policy;
	struct sched_param sched_param;
} pthread_attr_t;
#endif
#include <defines/pthread_attr_t_define.h>

typedef cond_t pthread_cond_t;

typedef struct condattr pthread_condattr_t;

/* In cases where default condition variable attributes are appropriate, the
 * macro PTHREAD_COND_INITIALIZER can be used to initialize condition variables
 * that are statically allocated. The effect shall be equivalent to dynamic
 * initialization by a call to pthread_cond_init() with parameter attr specified
 * as NULL, except that no error checks are performed.
 */
#define PTHREAD_COND_INITIALIZER COND_INIT_STATIC

typedef struct mutex pthread_mutex_t;

typedef struct mutexattr pthread_mutexattr_t;

typedef struct rwlock pthread_rwlock_t;

typedef struct pthread_rwlockattr {

} pthread_rwlockattr_t;


typedef size_t pthread_key_t;


typedef pthread_mutex_t pthread_once_t;

struct _pthread_barrier_t {
    unsigned int in;
    unsigned int round;
    unsigned int count;
    int istep;
    int pshared;
    int initialize_flag;
    int in_use;
    sem_t lock[2];
    pthread_mutex_t mutex_lock;
};

typedef  struct _pthread_barrierattr_t {
    int pshared;
} pthread_barrierattr_t;

typedef struct _pthread_barrier_t *pthread_barrier_t;

#define PTHREAD_ONCE_INIT PTHREAD_MUTEX_INITIALIZER

#define PTHREAD_CANCEL_ENABLE       0x0
#define PTHREAD_CANCEL_DISABLE      0x1

#define PTHREAD_CANCEL_DEFERRED     0x0
#define PTHREAD_CANCEL_ASYNCHRONOUS 0x1

__BEGIN_DECLS

/*
 * TODO: Implement!
 */
/* in own module */
extern int   pthread_attr_getguardsize(const pthread_attr_t *, size_t *);

extern int   pthread_attr_destroy(pthread_attr_t *);
extern int   pthread_attr_getdetachstate(const pthread_attr_t *, int *);
extern int   pthread_attr_getinheritsched(const pthread_attr_t *, int *);
extern int   pthread_attr_getschedparam(const pthread_attr_t *, struct sched_param *);
extern int   pthread_attr_getschedpolicy(const pthread_attr_t *, int *);
//extern int   pthread_attr_getscope(const pthread_attr_t *, int *);
extern int   pthread_attr_getstackaddr(const pthread_attr_t *, void **);
extern int   pthread_attr_getstacksize(const pthread_attr_t *, size_t *);
extern int   pthread_attr_getstack(pthread_attr_t *attr,
                                        void **stackaddr, size_t *stacksize);
extern int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr);
extern int   pthread_attr_init(pthread_attr_t *);
extern int   pthread_attr_setdetachstate(pthread_attr_t *, int);
//extern int   pthread_attr_setguardsize(pthread_attr_t *, size_t);
extern int   pthread_attr_setinheritsched(pthread_attr_t *, int);
extern int   pthread_attr_setschedparam(pthread_attr_t *, const struct sched_param *);
extern int   pthread_attr_setschedpolicy(pthread_attr_t *, int);
//extern int   pthread_attr_setscope(pthread_attr_t *, int);
//extern int   pthread_attr_setstackaddr(pthread_attr_t *, void *);
extern int   pthread_attr_setstacksize(pthread_attr_t *, size_t);
extern int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);

extern int   pthread_cancel(pthread_t);
extern void  pthread_cleanup_push(void (*)(void *), void *arg);
extern void  pthread_cleanup_pop(int);

extern int   pthread_cond_broadcast(pthread_cond_t *);
extern int   pthread_cond_destroy(pthread_cond_t *);
extern int   pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
extern int   pthread_cond_signal(pthread_cond_t *);
extern int   pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);
extern int   pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
extern int   pthread_condattr_getclock(const pthread_condattr_t */*restrict*/, clockid_t */*restrict*/);
extern int   pthread_condattr_setclock(pthread_condattr_t *, clockid_t);

extern int   pthread_condattr_destroy(pthread_condattr_t *);
extern int   pthread_condattr_getpshared(const pthread_condattr_t *, int *);
extern int   pthread_condattr_init(pthread_condattr_t *);
extern int   pthread_condattr_setpshared(pthread_condattr_t *, int);

extern int   pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
extern int   pthread_detach(pthread_t);
extern int   pthread_equal(pthread_t, pthread_t);
extern void  pthread_exit(void *);
//extern int   pthread_getconcurrency(void);
extern int   pthread_getschedparam(pthread_t, int *, struct sched_param *);
extern void *pthread_getspecific(pthread_key_t);
extern int   pthread_join(pthread_t, void **);

extern int   pthread_key_create(pthread_key_t *, void (*)(void *));
extern int   pthread_key_delete(pthread_key_t);

extern int   pthread_kill(pthread_t thread, int sig);

extern int   pthread_mutex_destroy(pthread_mutex_t *);
//extern int   pthread_mutex_getprioceiling(const pthread_mutex_t *, int *);
extern int   pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
extern int   pthread_mutex_lock(pthread_mutex_t *);
//extern int   pthread_mutex_setprioceiling(pthread_mutex_t *, int, int *);
extern int   pthread_mutex_trylock(pthread_mutex_t *);
extern int   pthread_mutex_unlock(pthread_mutex_t *);
extern int   pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);

extern int   pthread_mutexattr_destroy(pthread_mutexattr_t *);
//extern int   pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *, int *);
//extern int   pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *);
extern int   pthread_mutexattr_gettype(const pthread_mutexattr_t *, int *);
extern int   pthread_mutexattr_init(pthread_mutexattr_t *);
//extern int   pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);
//extern int   pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
extern int   pthread_mutexattr_settype(pthread_mutexattr_t *, int);

/* pthread_mutexattr_setprotocol, pthread_mutexattr_getprotocol - set and get
   protocol attribute of mutex attribute object (REALTIME THREADS)
*/
#define PTHREAD_PRIO_NONE     0x0
#define PTHREAD_PRIO_INHERIT  0x1
#define PTHREAD_PRIO_PROTECT  0x2
extern int   pthread_mutexattr_getprotocol(const pthread_mutexattr_t *, int *);
extern int   pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);

extern int   pthread_once(pthread_once_t *, void (*)(void));

extern int   pthread_rwlock_destroy(pthread_rwlock_t *);
extern int   pthread_rwlock_init(pthread_rwlock_t *, const pthread_rwlockattr_t *);
extern int   pthread_rwlock_rdlock(pthread_rwlock_t *);
extern int   pthread_rwlock_tryrdlock(pthread_rwlock_t *);
extern int   pthread_rwlock_trywrlock(pthread_rwlock_t *);
extern int   pthread_rwlock_unlock(pthread_rwlock_t *);
extern int   pthread_rwlock_wrlock(pthread_rwlock_t *);

//extern int   pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
//extern int   pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *, int *);
//extern int   pthread_rwlockattr_init(pthread_rwlockattr_t *);
//extern int   pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);

extern pthread_t pthread_self(void);

extern int   pthread_setcancelstate(int, int *);
extern int   pthread_setcanceltype(int, int *);

//extern int   pthread_setconcurrency(int);
extern int   pthread_setschedparam(pthread_t, int, const struct sched_param *);
extern int   pthread_setspecific(pthread_key_t, const void *);

extern void  pthread_testcancel(void);

extern int   pthread_setschedprio(pthread_t, int);

extern int   pthread_barrier_init(pthread_barrier_t *,
    const pthread_barrierattr_t *, unsigned int);
extern int   pthread_barrier_wait(pthread_barrier_t *);
extern int   pthread_barrier_destroy(pthread_barrier_t *);
extern int   pthread_barrierattr_init(pthread_barrierattr_t *);
extern int   pthread_barrierattr_destroy(pthread_barrierattr_t *);

/* _GNU_SOURCE */
extern int pthread_attr_setaffinity_np(pthread_attr_t *attr,
                                size_t cpusetsize, const cpu_set_t *cpuset);
extern int pthread_attr_getaffinity_np(const pthread_attr_t *attr,
                                size_t cpusetsize, cpu_set_t *cpuset);

extern int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
                                const cpu_set_t *cpuset);
extern int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize,
                                cpu_set_t *cpuset);

__END_DECLS

#endif /* PTHREAD_H_ */
