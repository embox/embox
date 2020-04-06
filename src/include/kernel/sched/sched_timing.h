/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_TIMING_H_
#define SCHED_TIMING_H_

#include <module/embox/kernel/sched/timing/timing.h>

struct sched_timing;
struct schedee;

extern void sched_timing_init(struct schedee *s);
extern clock_t sched_timing_get(struct schedee *s);
extern void sched_timing_start(struct schedee *s);
extern void sched_timing_stop(struct schedee *s);

#define SCHED_TIMING_INIT() \
	__SCHED_TIMING_INIT()

#endif /* SCHED_TIMING_H_ */
