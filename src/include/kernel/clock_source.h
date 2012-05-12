/**
 * @file
 * @brief API for registration of time's device
 *
 * @date 06.07.2011
 * @author Ilia Vaprol
 */

#ifndef KERNEL_CLOCK_SOURCE_H_
#define KERNEL_CLOCK_SOURCE_H_

#include <types.h>
#include <util/dlist.h>


typedef uint64_t cycle_t;

struct clock_source {
	uint32_t flags; /**< periodical or not */
	uint32_t precision;
};

struct clock_source_head {
	struct dlist_head lnk;
	struct clock_source *clock_source;
};

extern struct dlist_head clock_source_list;

/* register new clock_source at the system */
extern int clock_source_register(struct clock_source *cs);

/* unregister clock source at the system */
extern int clock_source_unregister(struct clock_source *cs);

/* return driver setting */
extern uint32_t clock_source_get_precision(struct clock_source *cs);

/* convert clock to usec */
extern useconds_t clock_source_clock_to_usec(struct clock_source *cs, clock_t cl);


#endif /* KERNEL_CLOCK_SOURCE_H_ */
