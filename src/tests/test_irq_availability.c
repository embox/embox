/**
 * \file test_irq_availability.c
 *
 * \date 28.01.2009
 * \author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include "types.h"
#include "common.h"
#include "timers.h"
#include "test_irq_availability.h"
#include "express_tests.h"

volatile static BOOL tick_happened;

static void test_timer_handler() {
	tick_happened = TRUE;
}

#define TEST_NAME "irq availability"
static char *get_test_name(){
    return TEST_NAME;
}

static int exec(){
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
    return tick_happened ? 0 : -1;
}

static EXPRESS_TEST_HANDLER exp_handler = {get_test_name, exec};
REGISTER_EXPRESS_TEST(&exp_handler);
