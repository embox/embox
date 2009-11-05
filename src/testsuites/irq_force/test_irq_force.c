/**
 * \file test_irq_force.c
 *
 * \date 02.11.2009
 * \author Alexey Fomin
 */

#include "common.h"
#include "express_tests.h"
#include "kernel/irq_ctrl.h"
#include "kernel/irq.h"

#define TEST_IRQ_NUM 10

DECLARE_EXPRESS_TEST("force irq", exec)

volatile static BOOL irq_happened;

static void test_irq_force_handler() {
	irq_happened = TRUE;
}

static int exec() {
	long i;
	IRQ_HANDLER old_irq_handler = irq_get_handler(TEST_IRQ_NUM);
	int old_irq_enabled = irq_ctrl_get_status(TEST_IRQ_NUM);

	irq_happened = FALSE;
	irq_set_handler(TEST_IRQ_NUM, (IRQ_HANDLER) test_irq_force_handler);
	irq_ctrl_force(TEST_IRQ_NUM);
	if (!irq_happened) {
		TRACE (" psr: 0x%08X ", get_psr());
	}
	irq_set_handler(TEST_IRQ_NUM, old_irq_handler);
	if (!old_irq_enabled) {
		irq_ctrl_disable_irq(TEST_IRQ_NUM);
	}
	return irq_happened ? 0 : -1;
}
