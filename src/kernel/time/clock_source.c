/**
 * @file
 * @brief API for registration of time's device
 *
 * @date 06.07.11
 * @author Ilia Vaprol
 * @author Alexander Kalmuk
 *         - timecounter
 */

#include <types.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#include <embox/unit.h>

#include <util/dlist.h>
#include <mem/misc/pool.h>

#include <kernel/clock_source.h>
#include <kernel/ktime.h>
#include <kernel/time/timecounter.h>

POOL_DEF(clock_source_pool, struct clock_source_head, OPTION_GET(NUMBER,clocks_quantity));
DLIST_DEFINE(clock_source_list);

static struct clock_source_head *clock_source_find(struct clock_source *cs) {
	struct clock_source_head *csh;

	struct dlist_head *tmp, *csh_lnk;

	dlist_foreach(csh_lnk,tmp,&clock_source_list) {
		csh = dlist_entry(csh_lnk, struct clock_source_head, lnk);
		if(cs == csh->clock_source) {
			return csh;
		}
	}

	return NULL;
}

int clock_source_register(struct clock_source *cs) {
	struct clock_source_head *csh;

	if (!cs) {
		return -EINVAL;
	}
	csh = (struct clock_source_head *) pool_alloc(&clock_source_pool);
	if (!csh) {
		return -EBUSY;
	}
	csh->clock_source = cs;

	dlist_add_prev(dlist_head_init(&csh->lnk), &clock_source_list);

	return ENOERR;
}

int clock_source_unregister(struct clock_source *cs) {
	struct clock_source_head *csh;

	if (!cs) {
		return -EINVAL;
	}
	if(NULL == (csh = clock_source_find(cs))) {
		return -EBADF;
	}

	return ENOERR;
}


uint32_t clock_source_get_precision(struct clock_source *cs) {
	return (uint32_t) cs->precision;
}

struct clock_source *clock_source_get_default(void) {
	assert(!dlist_empty(&clock_source_list));

	return ((struct clock_source_head *)clock_source_list.next)->clock_source;
}

useconds_t clock_source_clock_to_usec(struct clock_source *cs, clock_t cl) {
	return (useconds_t) (((useconds_t) cl) * cs->precision);
}


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

/* mult and shift used to correct clocks (e.g. with NTP). We try to choose
 * maximal shift, that guarantees 1 / (2^shift) precision. --Alexander */
#ifdef EXACT_CLOCKS_SCALE
/**
 * This code from linux/kernel/time/clocksource.c. (3.3.5 kernel)
 *
 * clocks_calc_mult_shift - calculate mult/shift factors for scaled math of clocks
 * @mult:	pointer to mult variable
 * @shift:	pointer to shift variable
 * @from:	frequency to convert from
 * @to:		frequency to convert to
 * @maxsec:	guaranteed runtime conversion range in seconds
 *
 * The function evaluates the shift/mult pair for the scaled math
 * operations of clocksources and clockevents.
 *
 * @to and @from are frequency values in HZ. For clock sources @to is
 * NSEC_PER_SEC == 1GHz and @from is the counter frequency. For clock
 * event @to is the counter frequency and @from is NSEC_PER_SEC.
 *
 * The @maxsec conversion range argument controls the time frame in
 * seconds which must be covered by the runtime conversion with the
 * calculated mult and shift factors. This guarantees that no 64bit
 * overflow happens when the input value of the conversion is
 * multiplied with the calculated mult factor. Larger ranges may
 * reduce the conversion accuracy by chosing smaller mult and shift
 * factors.
 */
void clocks_calc_mult_shift(uint32_t *mult, uint32_t *shift, uint32_t from,
		uint32_t to, uint32_t maxsec)
{
	uint64_t tmp;
	uint32_t sft, sftacc = 32;

	/*
	 * Calculate the shift factor which is limiting the conversion
	 * range:
	 */
	tmp = ((uint64_t)maxsec * from) >> 32;
	while (tmp) {
		tmp >>= 1;
		sftacc--;
	}

	/*
	 * Find the conversion shift/mult pair which has the best
	 * accuracy and fits the maxsec conversion range:
	 */
	for (sft = 32; sft > 0; sft--) {
		tmp = (uint64_t) to << sft;
		tmp += from / 2;
		do_div(tmp, from);
		if ((tmp >> sftacc) == 0)
			break;
	}
	*mult = tmp;
	*shift = sft;
}
#else

void clocks_calc_mult_shift(uint32_t *mult, uint32_t *shift, uint32_t from,
		uint32_t to, uint32_t maxsec) {
	*mult = to / from;
	*shift = 0;
}

#endif
