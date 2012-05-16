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
#include <embox/unit.h>

POOL_DEF(timecounter_pool, struct timecounter, OPTION_GET(NUMBER,timecounter_quantity));

struct timecounter *timecounter_alloc(void) {
	return pool_alloc(&timecounter_pool);
}


void timecounter_free(struct timecounter *tc) {
	pool_free(&timecounter_pool, tc);
}

void timecounter_init(struct timecounter *tc, const struct cyclecounter *cc,
		ns_t start_tstamp) {
	tc->cc = cc;
	tc->cycle_last = cc->read(cc);
	tc->nsec = start_tstamp;
}

ns_t timecounter_read(struct timecounter *tc) {
	cycle_t cycle_now;
	uint64_t nsec;

	cycle_now = tc->cc->read(tc->cc);
	nsec = cycles_to_ns(tc->cc, cycle_now - tc->cycle_last);
	tc->cycle_last = cycle_now;

	/* increment time by nanoseconds since last call */
	nsec += tc->nsec;
	tc->nsec = nsec;

	return nsec;
}

