/**
 * @file
 * @brief Defines semaphore structure and methods associated with it.
 *
 * @date 02.09.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SYNC_SEM_H_
#define KERNEL_THREAD_SYNC_SEM_H_

#include <kernel/thread/api.h>

struct sem {
	struct event event;
	int value;
	int max_value;
};

typedef struct sem sem_t;

extern void sem_init(sem_t *s, int val);
extern void sem_enter(sem_t *s);
extern void sem_leave(sem_t *s);

#endif /* KERNEL_THREAD_SYNC_SEM_H_ */

