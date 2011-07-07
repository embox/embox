/*
 * clock_source.h
 *
 *  Created on: 06.07.2011
 *      Author: hak1r
 */

#ifndef CLOCK_SOURCE_H_
#define CLOCK_SOURCE_H_

#include <types.h>
#include <lib/list.h>

struct clock_source;

extern void clock_source_register(struct clock_source **pcs, uint32_t flags, uint32_t precision);

extern uint32_t clock_source_get_HZ(void);

extern useconds_t clock_source_clock_to_usec(clock_t cl);

#endif /* CLOCK_SOURCE_H_ */
