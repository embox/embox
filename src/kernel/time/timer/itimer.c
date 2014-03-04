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
#include <stdio.h>

#include <kernel/time/itimer.h>
#include <hal/clock.h>

POOL_DEF(itimer_pool, struct itimer, OPTION_GET(NUMBER, itimer_quantity));

struct itimer *itimer_alloc(void) {
	return pool_alloc(&itimer_pool);
}

void itimer_free(struct itimer *it) {
	pool_free(&itimer_pool, it);
}

void itimer_init(struct itimer *it, struct clock_source *cs,
		time64_t start_tstamp) {
	assert(it);
	assert(cs);
	assert(cs->read);

	it->cs = cs;
	it->start_value = start_tstamp + cs->read(it->cs);
}

time64_t itimer_read(struct itimer *it) {
	assert(it);
	assert(it->cs);
	assert(it->cs->read);

	return it->cs->read(it->cs) - it->start_value;
}
