/**
 * @file   
 * @brief  pthread_barrier functions
 * @date   11.03.2020
 * @author Sobhan Mondal <inro20fdm@gmail.com>
 */

#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <util/err.h>

static void sem_post_multiple(sem_t *sem, int count) { 
    for (; count > 0; --count) {
        sem_post(sem);
    }
} 

/*
 * The pthread_barrier_init() function shall fail if:
 *
 * [EBUSY]
 *	   The implementation has detected an attempt to 
 *     reinitialize a barrier while it is in use.
 * [EINVAL]
 *     The value specified by count is equal to zero 
 *	   or the barrier is not defined.
 * [ENOMEM]
 *     Insufficient memory exists to initialize the barrier.
 *
 * On success, The pthread_barrier_init() function will return 0.
 * The pthread_barrier_init() function shall not return an 
 * error code of [EINTR].
 */

int pthread_barrier_init(pthread_barrier_t *barrier, 
	const pthread_barrierattr_t *attr, unsigned int count) {	
	pthread_barrier_t ib;

	if (NULL == barrier || 0 == count) {
		return EINVAL;
	}

	if (NULL != *barrier) {
        if (1 == (*barrier)->initialize_flag) {
            return EBUSY;
        }
    }

	if (NULL != (ib = (pthread_barrier_t) malloc(sizeof(*ib)))) {
		if (NULL != attr) {
			ib->pshared = attr->pshared;
		}
		else {
			ib->pshared = PTHREAD_PROCESS_PRIVATE;
		}

		ib->in = 0;
		ib->round = 0;
		ib->count = count;
		ib->istep = 0;
		ib->initialize_flag = 1;
		ib->in_use = 0;

		if (0 == sem_init(&(ib->lock[0]), ib->pshared, 0)) {
			if (0 == sem_init(&(ib->lock[1]), ib->pshared, 0)) {
				(*barrier) = ib;
				return 0;
			}
			sem_destroy(&(ib->lock[0]));
		}
		free(ib);
	}
	return ENOMEM;
}

/*
 * The pthread_barrier_wait() function shall fail if:
 *
 * [EINVAL]
 *     The value specified by barrier does not refer to an 
 *     initialized barrier object.
 *
 * On success, The pthread_barrier_wait() function will return 
 * PTHREAD_BARRIER_SERIAL_THREAD for a single (arbitrary) thread
 * synchronized at the barrier and 0 for each of the other threads.
 * The pthread_barrier_wait() function shall not return an 
 * error code of [EINTR].
 */

int pthread_barrier_wait(pthread_barrier_t *barrier) {	
	int result;
	int step;
	pthread_barrier_t ib;
	pthread_mutex_t mutex_lock;

	if (NULL == barrier) {
		return EINVAL;
	}

	if (NULL != (*barrier)) {
        if (0 == (*barrier)->initialize_flag) {
            return EINVAL;
        }
    }

	ib = (*barrier);
	if (0 == (ib->round) % 2) {
		ib->istep = 0;
	} else {
		ib->istep = 1;
	}
	step = ib->istep;

	pthread_mutex_init(&mutex_lock, NULL); 
	pthread_mutex_lock(&mutex_lock); 
	(ib->in)++;
	pthread_mutex_unlock(&mutex_lock); 
	pthread_mutex_destroy(&mutex_lock); 

	if (ib->in == ib->count) {
		ib->in = 0;
		(ib->round)++;
		if (ib->count > 1) { 
			sem_post_multiple(&(ib->lock[step]), ib->count - 1);
		}
		result = PTHREAD_BARRIER_SERIAL_THREAD;
	} else {
		result = sem_wait(&(ib->lock[step]));		
	}
	return result;
}

/*
 * pthread_barrier_destroy() function shall fail if:
 *
 * [EBUSY]
 *	   The implementation has detected an attempt to 
 *     destroy a barrier while it is in use.
 * [EINVAL]
 *     The value specified by barrier is invalid.
 *
 * On success, The pthread_barrier_destroy() function will return 0.
 * The pthread_barrier_destroy() function shall not 
 * return an error code of [EINTR].
 */

int pthread_barrier_destroy(pthread_barrier_t *barrier) {	
	int result = 0;
	pthread_barrier_t ib;

	if (NULL == barrier) {
		return EINVAL;
	}

	if (NULL != (*barrier)) {
        if (0 == (*barrier)->initialize_flag) {
            return EINVAL;
        }
        if (1 == (*barrier)->in_use) {
			return EBUSY;
		}
    }

	ib = (*barrier);
	(*barrier) = NULL;

	result = sem_destroy(&(ib->lock[0]));
	if (0 == result) {
		result = sem_destroy(&(ib->lock[1]));
		if (0 == result) {
			free(ib);
			return 0;
		}
		sem_init(&(ib->lock[0]), ib->pshared, 0);
	}
	(*barrier) = ib;
	return result;
}

/*
 * pthread_barrierattr_destroy() function shall fail if:
 *
 * [EINVAL]
 *     The value specified by attr is invalid.
 *
 * On success, The pthread_barrierattr_destroy() function will return 0.
 * The pthread_barrierattr_destroy() function shall not return an error code of [EINTR].
 */

int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) {	
	int result = 0;

	if (NULL == attr) {
		return EINVAL;
	} else {
		free(attr);
		attr = NULL;
	}
	return result;
}
