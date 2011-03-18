/**
 * @file
 *
 * @date 02.11.09
 * @author Alexey Fomin
 * @author Eldar Abusalimov
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <embox/test.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>
#include <test/misc.h>

#define TEST_IRQ_NR 10

EMBOX_TEST(run);

static int run(void) {
	return test_misc_irq_force(TEST_IRQ_NR);
}
