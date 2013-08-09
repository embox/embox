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

extern void sched_timing_init(struct thread *t);

extern clock_t sched_timing_get(struct thread *t);

extern void sched_timing_start(struct thread *t, clock_t cur_time);

extern void sched_timing_stop(struct thread *t, clock_t cur_time);

#endif /* SCHED_TIMING_H_ */
