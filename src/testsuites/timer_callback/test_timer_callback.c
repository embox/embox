/**
 * \file test_irq_availability.c
 *
 * \date 28.01.2009
 * \author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */
#include "autoconf.h"
#include "common.h"
#include "kernel/timers.h"
#include "express_tests.h"


DECLARE_EXPRESS_TEST("timer callback", exec, TEST_TIMER_CALLBACK_ON_BOOT_ENABLE, NULL);

volatile static bool tick_happened;

static void test_timer_handler() {
	tick_happened = true;
}

static int exec() {
	uint32_t id, ticks;
	long i;
	id = 17;
	ticks = 2;
	// (timer value changes means ok)
	tick_happened = false;
	set_timer(id, ticks, test_timer_handler);
	for (i = 0; i < (1 << 20); i++) {
		if (tick_happened)
			break;
	}
	close_timer(id);

	return tick_happened ? 0 : -1;
}
