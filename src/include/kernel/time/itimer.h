/**
 * @file
 *
 * @brief
 *
 * @date 15.05.2012
 * @author Anton Bondarev
 */

#ifndef KERNEL_TIME_TIMECOUNTER_H_
#define KERNEL_TIME_TIMECOUNTER_H_

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>

struct itimer {
	time64_t start_value;
	struct clock_source *cs;
};

extern void itimer_init(struct itimer *it,
		struct clock_source *cs, time64_t start_tstamp);

extern time64_t itimer_read(struct itimer *it);

extern struct itimer *itimer_alloc(void);
extern void itimer_free(struct itimer *it);

#endif /* KERNEL_TIME_TIMECOUNTER_H_ */
