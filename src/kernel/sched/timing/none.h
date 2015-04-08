/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_TIMING_NONE_H_
#define SCHED_TIMING_NONE_H_

#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/sched/sched_timing.h>

struct sched_timing {
	EMPTY_STRUCT_BODY
};

static inline void sched_timing_init(struct schedee *t) { }

static inline clock_t sched_timing_get(struct schedee *t) {
	return 0;
}

static inline void sched_timing_start(struct schedee *s) { }

static inline void sched_timing_stop(struct schedee *s) { }

#endif /* SCHED_TIMING_NONE_H_ */
