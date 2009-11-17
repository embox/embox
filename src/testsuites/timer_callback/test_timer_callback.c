/**
 * \file test_irq_availability.c
 *
 * \date 28.01.2009
 * \author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include "common.h"
#include "kernel/timers.h"
#include "express_tests.h"

DECLARE_SMART_EXPRESS_TEST("timer callback", exec, FALSE, NULL);

volatile static BOOL tick_happened;

static void test_timer_handler() {
	tick_happened = TRUE;
}

static int exec() {
	UINT32 id, ticks;
	long i;
	id = 17;
	ticks = 2;
	// (timer value changes means ok)
	tick_happened = FALSE;
	set_timer(id, ticks, test_timer_handler);
	for (i = 0; i < (1 << 20); i++) {
		if (tick_happened)
			break;
	}
	close_timer(id);
	if(!tick_happened) {
		TRACE (" psr: 0x%08X ", get_psr());
	}
	return tick_happened ? 0 : -1;
}
