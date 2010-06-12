/**
 * @file
 *
 * @date 02.11.2009
 * @author Eldar Abusalimov
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <embox/test.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>
#include <test/misc.h>

#define RECURSION_DEPTH 32

#define TEST_IRQ_NR 10

EMBOX_TEST(run);

static int test_cb(void) {
	return test_misc_recursion(RECURSION_DEPTH);
}

static int run(void) {
	return test_misc_irq_force_callback(TEST_IRQ_NR, test_cb);
}
