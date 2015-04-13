/*
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef AFFINITY_H_
#define AFFINITY_H_

#include <module/embox/kernel/sched/affinity/affinity.h>

struct affinity;
extern void sched_affinity_init(struct affinity *a);
extern int sched_affinity_check(struct affinity *a, int mask);
extern void sched_affinity_set(struct affinity *a, int mask);
extern int sched_affinity_get(struct affinity *a);

#endif /* AFFINITY_H_ */
