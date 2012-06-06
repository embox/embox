/**
 * @file
 *
 * @brief
 *
 * @date 15.05.2012
 * @author Anton Bondarev
 */

#include <embox/unit.h>
#include <mem/misc/pool.h>

#include <kernel/time/timecounter.h>

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

	old_jiffies = tc->cs->read();
	tc->cycle_last = tc->cs->cc->read(tc->cs->cc);
	tc->jiffies_last = tc->cs->read();

	return (old_jiffies == tc->jiffies_last ? 1 : 0);
}

void timecounter_init(struct timecounter *tc, struct clock_source *cs,
		ns_t start_tstamp) {
	int jiff;

	assert(tc);
	assert(cs);
	assert(cs->dev);
	assert(cs->cc);

	tc->cs = cs;

	jiff = cs->read();
	while (cs->read() == jiff);
	tc->jiffies_last = cs->read();
	tc->cycle_last = cs->cc->read(cs->cc);

	tc->nsec = start_tstamp;
}

ns_t timecounter_read(struct timecounter *tc) {
	cycle_t cycle_old;
	uint32_t jiffies_old;
	ns_t nsec;
	struct clock_source *cs;
	int res;
	int ticks_per_jiff;

	assert(tc);
	assert(tc->cs);
	cs = tc->cs;

	jiffies_old = tc->jiffies_last;
	cycle_old = tc->cycle_last;

	/* try to read between two nearby jiffies */
	res = protect_read_cycles(tc);

	nsec = 0;

	if (!res) {
		tc->cycle_last = 0;
	}

	ticks_per_jiff = ((cs->dev->resolution + cs->resolution / 2) / cs->resolution) - 1;

	if (cycle_old > tc->cycle_last) {
		tc->jiffies_last++;
		cycle_old -= ticks_per_jiff;
	}

	nsec += cycles_to_ns(cs->cc, (tc->jiffies_last - jiffies_old) * ticks_per_jiff);
	nsec += cycles_to_ns(cs->cc, tc->cycle_last - cycle_old);

#ifdef TIMECOUNTER_DEBUG
	printf("c: %d %d\n", (int) tc->cycle_last, (int) cycle_old);
	printf("j: %d %d\n", (int) tc->jiffies_last, (int) jiffies_old);
	printf("ns: %d \n\n", (int) nsec);
#endif

	return (tc->nsec + nsec);
}

