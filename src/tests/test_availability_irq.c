/*
 * test_availability_irq.c
 *
 *  Created on: 28.01.2009
 *      Authors: abatyukov, afomin, eabysalimov
 */
#include "types.h"
#include "common.h"
#include "timers.h"
#include "test_availability_irq.h"

volatile static BOOL tick_happened ;

static void test_timer_handler(){
	tick_happened = TRUE;
}

int test_availability_irq() {
	UINT32 id,ticks;
	long i;
	//TRACE ("Testing availability_irq...   ");
	id = 23;
	ticks = 2;
	// (timer value changes means ok)
	tick_happened = FALSE;
	set_timer(id, ticks,test_timer_handler);
	for (i = 0 ;i<(2<<20);i++){
		if (tick_happened)
			break;
	}
	close_timer(id);
	//if (TRUE == tick_happened)
	//	TRACE ("OK\n");
	//else TRACE ("failed\n");
	return 0;
}
