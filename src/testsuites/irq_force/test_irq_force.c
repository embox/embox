/**
 * \file test_irq_force.c
 *
 * \date 02.11.2009
 * \author Alexey Fomin
 */

#include "autoconf.h"
#include "common.h"
#include "express_tests.h"
#include "hal/irq_ctrl.h"
#include "kernel/irq.h"

#define TEST_IRQ_NUM 10


DECLARE_EXPRESS_TEST("force irq", exec, TEST_FORCE_IRQ_ON_BOOT_ENABLE, NULL);

volatile static BOOL irq_happened;

static void test_irq_force_handler() {
	irq_happened = TRUE;
}

static int exec() {
	IRQ_INFO irq_info = {TEST_IRQ_NUM, test_irq_force_handler, TRUE};

	irq_happened = FALSE;

	if (!irq_set_info(&irq_info)) {
		TRACE("Unable to set irq handler\n");
		return -3;
	}

	irqc_force(irq_info.irq_num);

	if (!irq_happened) {
		TRACE (" psr: 0x%08X ", get_psr());
	}

	irq_set_info(&irq_info);
	return irq_happened ? 0 : -1;
}
