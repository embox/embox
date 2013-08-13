/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_NO_TIME_H_
#define SCHED_NO_TIME_H_


struct sched_timing {
};

typedef struct sched_timing __sched_timing_t;

static inline void sched_timing_init(struct thread *t) {
}

static inline clock_t sched_timing_get(struct thread *t) {
	return 0;
}

static inline void sched_timing_start(struct thread *t, clock_t cur_time) {
}

static inline void sched_timing_stop(struct thread *t, clock_t cur_time) {
}

#endif /* SCHED_NO_TIME_H_ */
