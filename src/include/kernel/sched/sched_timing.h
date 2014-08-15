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

struct schedee;

extern void sched_timing_init(struct schedee *s);
extern clock_t sched_timing_get(struct schedee *s);
extern void sched_timing_start(struct schedee *s);
extern void sched_timing_stop(struct schedee *s);

#endif /* SCHED_TIMING_H_ */
