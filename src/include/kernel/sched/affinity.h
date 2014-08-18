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

struct schedee;

extern int sched_affinity_check(struct schedee *s, int mask);
extern void sched_affinity_init(struct schedee *s);
extern void sched_affinity_set(struct schedee *s, int mask);
extern int sched_affinity_get(struct schedee *s);

#endif /* AFFINITY_H_ */
