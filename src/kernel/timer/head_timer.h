/**
 * @file
 *
 * @date 15.07.10
 * @author Fedor Burdun
 */

#ifndef TIMER_IMPL_H_
#define TIMER_IMPL_H_

#include <types.h>
#include <kernel/irq.h>

#ifndef __ASSEMBLER__

struct sys_tmr {
	struct list_head *next, *prev;
	uint32_t   load;
	uint32_t   cnt;
	TIMER_FUNC handler;
	void       *args;
	bool       is_preallocated;
};


#endif /*__ASSEMBLER__*/
#endif /* TIMER_IMPL_H_ */

