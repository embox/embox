/**
 * @file
 *
 * @brief
 *
 * @date 15.05.2012
 * @author Anton Bondarev
 */
#include <mem/misc/pool.h>
#include <kernel/time/timecounter.h>
#include <kernel/clock_event.h>
#include <kernel/irq.h>
#include <embox/unit.h>
#include <stdio.h>

POOL_DEF(timecounter_pool, struct timecounter, OPTION_GET(NUMBER,timecounter_quantity));

struct timecounter *timecounter_alloc(void) {
	return pool_alloc(&timecounter_pool);
}

void timecounter_free(struct timecounter *tc) {
	pool_free(&timecounter_pool, tc);
}

/**
 * Try to read cyclecounter's value between two nearby jiffies.
 */
static int protect_read_cycles(struct timecounter *tc) {
	int old_jiffies;

	old_jiffies = tc->dev->get_jiffies();
	tc->cycle_last = tc->cc->read(tc->cc);
	tc->jiffies_last = tc->dev->get_jiffies();

	return (old_jiffies == tc->jiffies_last ? 1 : 0);
}

void timecounter_init(struct timecounter *tc, const struct clock_event_device *dev,
		ns_t start_tstamp) {
	int jiff;

	assert(tc);
	assert(dev);
	assert(dev->cs);
	assert(dev->cs->cc);

	tc->dev = dev;
	tc->cc = dev->cs->cc;

	jiff = dev->get_jiffies();
	while (dev->get_jiffies() == jiff);
	tc->jiffies_last = dev->get_jiffies();
	tc->cycle_last = tc->cc->read(tc->cc);

	tc->nsec = start_tstamp;
}

ns_t timecounter_read(struct timecounter *tc) {
	cycle_t cycle_old;
	uint32_t jiffies_old;
	ns_t nsec;
	int res;

	assert(tc);
	assert(tc->dev);

	jiffies_old = tc->jiffies_last;
	cycle_old = tc->cycle_last;

	/* if we can read between two nearby jiffies */
	res = protect_read_cycles(tc);

	nsec = 0;

	if (res && cycle_old > tc->cycle_last) {
		tc->jiffies_last++;
	}

	if (!res) {
		tc->cycle_last = 0;
	}

	nsec += cycles_to_ns(tc->cc, (tc->jiffies_last - jiffies_old) * cedev_get_ticks_per_jiff(tc->dev));
	nsec += cycles_to_ns(tc->cc, tc->cycle_last - cycle_old);

#ifdef TIMECOUNTER_DEBUG
	printf("c: %d %d\n", (int) tc->cycle_last, (int) cycle_old);
	printf("j: %d %d\n", (int) tc->jiffies_last, (int) jiffies_old);
	printf("ns: %d \n\n", (int) nsec);
#endif

	return (tc->nsec + nsec);
}

