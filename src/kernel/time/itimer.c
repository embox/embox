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

void itimer_init(struct itimer *it, const struct clock_source *cs,
		ns_t start_tstamp) {
	int s;
	assert(it);
	assert(cs);

	it->cs = cs;
	it->start_value = start_tstamp + cs->read(&s);
}

ns_t itimer_read(struct itimer *it) {
	int f;
	ns_t s;
	s= it->cs->read(&f);
	printf("%d ",f);
	return s - it->start_value;
}
