/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_TIMING_H_
#define SCHED_TIMING_H_

#include <time.h>

#include <module/embox/kernel/sched/strategy/timing_api.h>

typedef __sched_timing_t sched_timing_t;

struct thread;

extern void sched_timing_init(struct thread *t);
extern void sched_timing_switch(struct thread *prev, struct thread *next);
extern clock_t sched_timing_get(struct thread *t);

#endif /* SCHED_TIMING_H_ */
