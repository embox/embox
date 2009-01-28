/*
 * test_availability_irq.c
 *
 *  Created on: 28.01.2009
 *      Authors: abatyukov, afomin, eabysalimov
 */
#include "timers.h"
volatile BOOL tick_happened;

static void test_timer_handler(){
	tick_happened = TRUE;
}

int test_availability_irq() {
	UINT32 id,ticks;
	long i;
	TRACE ("Testing availability_irq...   ");
	id = 23;
	ticks = 2;
	// (timer value changes means ok)
	set_timer(id, ticks,test_timer_handler);
	for (i = 0 ;i<(2<<20);i++){
		if (tick_happened) TRACE ("OK\n");
		else TRACE ("failed\n");
	}
	close_timer(id);
	return 0;
}
