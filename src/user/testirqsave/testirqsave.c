/**
 * \file testmem.c
 * \date Jul 24, 2009
 * \author afomin
 */

#include "shell_command.h"
#include "kernel/irq_ctrl.h"
#include "kernel/irq.h"

#define COMMAND_NAME     "testirqsave"
#define COMMAND_DESC_MSG "checks whether irq_save disables irq's"
#define HELP_MSG         "Usage: tesirqsave [-h] [-i irq_num]"
static const char *man_page =
#include "testirqsave_help.inc"
;

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page)
;

typedef void TEST_MEM_FUNC(WORD *addr, long int amount);

static void testirqsave_handler() {
	TRACE("*** Running IRQ handler ***\n");
}
static int exec(int argsc, char **argsv) {
	IRQ_HANDLER old_irq_handler;
	int old_irq_enabled;
	unsigned long old_psr;
	int i;
	int irq_num = 10;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "i:h");
		switch (nextOption) {
		case 'h':
			show_man_page();
			return 0;
		case 'i':
			// Key -i for irq_num
			// Can be in hex and in decimal

			if ((optarg == NULL) || // no irq_num after -i key specified
					((!sscanf(optarg, "0x%x", &irq_num)) // irq_num not in hex
							&& (!sscanf(optarg, "%d", (int *) &irq_num)))) { // irq_num not in decimal
				LOG_ERROR("testirqsave: -i: irq_num in hex or in decimal expected.\n");
				show_help();
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	TRACE("irq_num = 0x%x\n", irq_num);

	old_irq_handler = irq_get_handler(irq_num);
	old_irq_enabled = irq_ctrl_get_status(irq_num);
	irq_set_handler(irq_num, (IRQ_HANDLER) testirqsave_handler);

	old_psr = local_irq_save();
	TRACE("after irq_save():\n");

	TRACE("forcing irq...\n");
	irq_ctrl_force(irq_num);

	TRACE("\nafter irq_restore():\n");
	local_irq_restore(old_psr);

	irq_set_handler(irq_num, old_irq_handler);
	if (!old_irq_enabled) {
		irq_ctrl_disable_irq(irq_num);
	}

	return 0;
}
