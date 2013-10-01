/**
 * @file
 * @brief
 *
 * @date   27.03.2013
 * @author Anton Bulychev
 */

#include <errno.h>
#include <pthread.h>

#include <kernel/thread.h>
#include <err.h>

/*
int pthread_attr_destroy(pthread_attr_t *attr) {
	return -ENOSYS;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
	return -ENOSYS;
}

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize) {
	return -ENOSYS;
}

int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched) {
	return -ENOSYS;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param) {
	return -ENOSYS;
}

int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) {
	return -ENOSYS;
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope) {
	return -ENOSYS;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr) {
	return -ENOSYS;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
	return -ENOSYS;
}

int pthread_attr_init(pthread_attr_t *attr) {
	return -ENOSYS;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
	return -ENOSYS;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
	return -ENOSYS;
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
	return -ENOSYS;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param) {
	return -ENOSYS;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
	return -ENOSYS;
}

int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
	return -ENOSYS;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr) {
	return -ENOSYS;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	return -ENOSYS;
}

int pthread_cancel(pthread_t thread) {
	return -ENOSYS;
}

void pthread_cleanup_push(void (*routine)(void *), void *arg) {

}

void pthread_cleanup_pop(int execute) {

}
*/

int pthread_cond_broadcast(pthread_cond_t *cond) {
	cond_broadcast(cond);

	return ENOERR;
}

int pthread_cond_destroy(pthread_cond_t *cond) {
	cond_destroy(cond);

	return ENOERR;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
	cond_init(cond, attr);

	return ENOERR;
}

int pthread_cond_signal(pthread_cond_t *cond) {
	cond_signal(cond);

	return ENOERR;
}

/*
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
	return -ENOSYS;
}
*/

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	cond_wait(cond, mutex);

	return ENOERR;
}

int pthread_condattr_init(pthread_condattr_t *attr) {
	condattr_init(attr);

	return ENOERR;
}


int pthread_condattr_destroy(pthread_condattr_t *attr) {
	condattr_destroy(attr);

	return ENOERR;
}

int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared) {
	condattr_getpshared(attr, pshared);

	return ENOERR;
}


int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
	condattr_setpshared(attr, pshared);

	return ENOERR;
}


int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg) {
	struct thread *t;

	if(!start_routine) {
		return -EAGAIN;
	}

	t = thread_create(0, start_routine, arg);
	if(err(t)) {
		/*
		 * The pthread_create() function will fail if:
		 *
		 * [EAGAIN]
		 *     The system lacked the necessary resources to create another thread, or the system-imposed limit on the total number of threads in a process PTHREAD_THREADS_MAX would be exceeded.
		 * [EINVAL]
		 *     The value specified by attr is invalid.
		 * [EPERM]
		 *     The caller does not have appropriate permission to set the required scheduling parameters or scheduling policy.
		 *
		 * The pthread_create() function will not return an error code of [EINTR].
		 */
		return -EAGAIN;
	}
	*thread = t;

	return ENOERR;
}

int pthread_detach(pthread_t thread) {
	return thread_detach(thread);
}

int pthread_equal(pthread_t t1, pthread_t t2) {
	return t1 == t2;
}

void pthread_exit(void *value_ptr) {
	thread_exit(value_ptr);
}

/*
int pthread_getconcurrency(void) {
	return -ENOSYS;
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param) {
	return -ENOSYS;
}

void *pthread_getspecific(pthread_key_t key) {
	return NULL;
}
*/

int pthread_join(pthread_t thread, void **value_ptr) {
	return thread_join(thread, value_ptr);
}

/*
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
	return -ENOSYS;
}

int pthread_key_delete(pthread_key_t key) {
	return -ENOSYS;
}
*/

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	return ENOERR;
}

/*
int pthread_mutex_getprioceiling(const pthread_mutex_t *mutex, int *prioceiling) {
	return -ENOSYS;
}
*/

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
	mutex_init_default(mutex, attr);

	return ENOERR;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	return mutex_lock(mutex);
}

/*
int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling, int *old_ceiling) {
	return -ENOSYS;
}
*/

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	return mutex_trylock(mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	return mutex_unlock(mutex);
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
	return mutexattr_destroy(attr);
}

/*
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling) {
	return -ENOSYS;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol) {
	return -ENOSYS;
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared) {
	return -ENOSYS;
}
*/

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
	return mutexattr_gettype(attr, type);
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
	return mutexattr_init(attr);
}

/*
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling) {
	return -ENOSYS;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
	return -ENOSYS;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
	return -ENOSYS;
}
*/

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
	return mutexattr_settype(attr, type);
}

/*
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
	return -ENOSYS;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) {
	return -ENOSYS;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
	return -ENOSYS;
}

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *rwlock, int *pshared) {
	return -ENOSYS;
}

int pthread_rwlockattr_init(pthread_rwlockattr_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *rwlock, int pshared) {
	return -ENOSYS;
}
*/

pthread_t pthread_self(void) {
	return thread_self();
}

/*
int pthread_setcancelstate(int state, int *oldstate) {
	return -ENOSYS;
}

int pthread_setcanceltype(int type, int *oldtype) {
	return -ENOSYS;
}

int pthread_setconcurrency(int new_level) {
	return -ENOSYS;
}
*/
int pthread_setschedparam(pthread_t thread, int policy,
		const struct sched_param *param) {
	return thread_set_priority(thread, param->sched_priority);
}

/*
int pthread_setspecific(pthread_key_t key, const void *value) {
	return -ENOSYS;
}

void pthread_testcancel(void) {

}
*/
