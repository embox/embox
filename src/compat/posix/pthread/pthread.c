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
#include <kernel/thread/types.h>
#include <kernel/thread/thread_stack.h>
#include <util/err.h>


int pthread_attr_destroy(pthread_attr_t *attr) {
	return ENOERR;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
	*detachstate = attr->flags & THREAD_FLAG_DETACHED;
	return ENOERR;
}

/*
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize) {
	return -ENOSYS;
}
*/
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched) {
	*inheritsched = attr->flags & THREAD_FLAG_PRIORITY_INHERIT;

	return ENOERR;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param) {
	param->sched_priority = attr->sched_param.sched_priority;

	return ENOERR;
}

int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) {
	*policy = attr->policy;

	return ENOERR;
}

/*
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope) {
	return -ENOSYS;
}
*/
int pthread_attr_init(pthread_attr_t *attr) {
	attr->flags = 0;

	if (pthread_attr_setdetachstate(attr, 0)) {
		return -EINVAL;
	}
	if (pthread_attr_setinheritsched(attr, THREAD_FLAG_PRIORITY_INHERIT)) {
		return -EINVAL;
	}

	attr->policy = SCHED_OTHER;
	attr->sched_param.sched_priority = SCHED_PRIORITY_NORMAL;

	return ENOERR;
}

int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr) {
	attr->stack_size = thread_stack_get_size(thread);
	attr->stack = thread_stack_get(thread);
	attr->flags = 0;
	if (thread->state & TS_DETACHED) {
		attr->flags |= THREAD_FLAG_DETACHED;
	}

	return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
	if (detachstate) {
		attr->flags |= THREAD_FLAG_DETACHED;
	} else {
		attr->flags &= ~THREAD_FLAG_DETACHED;
	}
	return ENOERR;
}

/*
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
	return -ENOSYS;
}
*/

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
	if (inheritsched) {
		attr->flags |= THREAD_FLAG_PRIORITY_INHERIT;
	} else {
		attr->flags &= ~THREAD_FLAG_PRIORITY_INHERIT;
	}
	return ENOERR;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param) {
	//TODO move copy to other place
	attr->sched_param.sched_priority = param->sched_priority;

	return -ENOSYS;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
	attr->policy = policy;

	return ENOERR;
}

/*
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
	return -ENOSYS;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr) {
	return -ENOSYS;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	return -ENOSYS;
}
*/


int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg) {
	struct thread *t;
	pthread_attr_t def_attr;
	const pthread_attr_t *pattr;
	unsigned int flags;
	int detached, inherit, policy;
	struct sched_param sched_param;

	if (!start_routine) {
		return -EAGAIN;
	}
	if (NULL == attr) {
		pthread_attr_init(&def_attr);
		pattr = &def_attr;
	} else {
		pattr = attr;
	}
	if (0 != pthread_attr_getdetachstate(pattr, &detached)) {
		return -EINVAL;
	}
	if (0 != pthread_attr_getinheritsched(pattr, &inherit)) {
		return -EINVAL;
	}

	flags = detached | inherit | THREAD_FLAG_SUSPENDED;

	t = thread_create(flags, start_routine, arg);
	if (ptr2err(t)) {
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
		if (NULL == attr) {
			pthread_attr_destroy(&def_attr);
		}
		return -EAGAIN;
	}

	pthread_attr_getschedpolicy(pattr, &policy);
	pthread_attr_getschedparam(pattr, &sched_param);
	pthread_setschedparam(t, policy, &sched_param);

	thread_launch(t);
	*thread = t;

	if (NULL == attr) {
		pthread_attr_destroy(&def_attr);
	}
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
*/

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param) {
	*policy = thread->policy;
	param->sched_priority = schedee_priority_get(&thread->schedee);

	return ENOERR;
}

int pthread_join(pthread_t thread, void **value_ptr) {
	return thread_join(thread, value_ptr);
}


int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
	if((NULL == init_routine) || (NULL == once_control)) {
		return -EINVAL;
	}
	if(pthread_mutex_trylock(once_control)) {
		return 0;
	}
	init_routine();

	return 0;
}

pthread_t pthread_self(void) {
	return thread_self();
}

/*

int pthread_setconcurrency(int new_level) {
	return -ENOSYS;
}
*/
int pthread_setschedparam(pthread_t thread, int policy,
		const struct sched_param *param) {
	assertf((policy != SCHED_FIFO && policy != SCHED_RR) ||
			param->sched_priority >= 200, "In current realization you must "
			"use SCHED_FIFO and SCHED_RR only with priority more or equal 200");

	thread->policy = policy;
	return schedee_priority_set(&thread->schedee, param->sched_priority);
}

int pthread_setschedprio(pthread_t thread, int prio) {
	return schedee_priority_set(&thread->schedee, prio);
}

