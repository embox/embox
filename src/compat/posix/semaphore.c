/**
 * @file
 *
 * @brief
 *
 * @date 26.09.2013
 * @author Vita Loginova
 */

#include <errno.h>
#include <semaphore.h>
#include <kernel/thread/sync/semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value) {
	semaphore_init(sem, value);

	return ENOERR;
}

int sem_destroy(sem_t *sem) {
	return ENOERR;
}

int sem_getvalue(sem_t *restrict sem, int *restrict sval) {
	return semaphore_getvalue(sem, sval);
}

int sem_post(sem_t *sem) {
	semaphore_leave(sem);

	return ENOERR;
}

int sem_trywait(sem_t *sem) {
	return semaphore_tryenter(sem);
}

int sem_wait(sem_t *sem) {
	semaphore_enter(sem);

	return ENOERR;
}
