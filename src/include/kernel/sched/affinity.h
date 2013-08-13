/*
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef AFFINITY_H_
#define AFFINITY_H_


#include <module/embox/kernel/sched/strategy/affinity_api.h>

typedef __affinity_t affinity_t;

struct thread;

extern int sched_affinity_check(struct thread *t, int mask);
extern void sched_affinity_init(struct thread *t);
extern void sched_affinity_set(struct thread *t, int mask);
extern int sched_affinity_get(struct thread *t);

#endif /* AFFINITY_H_ */
