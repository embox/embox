/**
 * \file test_irq_force.c
 *
 * \date 02.11.2009
 * \author Alexey Fomin
 */

#include "common.h"
#include "embox/test.h"
#include "hal/irq_ctrl.h"
#include "kernel/irq.h"
#include "asm/sys.h"

#define TEST_IRQ_NUM 10

EMBOX_TEST(LOCAL_IRQ_SAVE, exec, NULL);

volatile static bool irq_happened;

static void test_local_irq_save_handler(int irq_num, void *dev_id, struct pt_regs *regs) {
	irq_happened = true;
}

static int exec(int argc, char** argv) {
	IRQ_INFO irq_info = {TEST_IRQ_NUM, test_local_irq_save_handler, NULL, NULL, 0, true};
	//int old_irq_enabled;
	unsigned long old_psr;

	irq_set_info(&irq_info);

	old_psr = local_irq_save();

	irqc_force(irq_info.irq_num);
	if (irq_happened) {
		return -1;
	}

	local_irq_restore(old_psr);
	if (!irq_happened) {
		return -1;
	}

	irq_set_info(&irq_info);

	return 0;
}
