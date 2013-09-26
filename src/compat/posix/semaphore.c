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
