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

struct runnable;

extern int sched_affinity_check(struct runnable *r, int mask);
extern void sched_affinity_init(struct runnable *r);
extern void sched_affinity_set(struct runnable *r, int mask);
extern int sched_affinity_get(struct runnable *r);

#endif /* AFFINITY_H_ */
