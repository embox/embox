/**
 * @file
 * @brief
 *
 * @date 06.07.2011
 * @author Ilia Vaprol
 */

#ifndef CLOCK_SOURCE_H_
#define CLOCK_SOURCE_H_

#include <types.h>
#include <lib/list.h>

struct clock_source {
	uint32_t flags; // periodical or not
	uint32_t precision;
	struct list_head *timers_list;
};

/* register new clock_source at system */
extern int clock_source_register(struct clock_source *cs);

/* return driver setting */
extern uint32_t clock_source_get_HZ(void);

/* convert clock to usec */
extern useconds_t clock_source_clock_to_usec(clock_t cl);

/* return system timers_list at system (from driver) */
extern struct list_head * clock_source_get_timers_list(void);

#endif /* CLOCK_SOURCE_H_ */
