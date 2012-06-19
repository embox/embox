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

#include <kernel/clock_source.h>
#include <kernel/time/ktime.h>

struct itimer {
	ns_t start_value;
	const struct clock_source *cs;
};

extern void itimer_init(struct itimer *it,
		const struct clock_source *cs, ns_t start_tstamp);

extern ns_t itimer_read(struct itimer *it);

extern struct itimer *itimer_alloc(void);
extern void itimer_free(struct itimer *it);

#endif /* KERNEL_TIME_TIMECOUNTER_H_ */
