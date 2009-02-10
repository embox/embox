/*
 * main.c
 *
 *  Created on: Nov 27, 2008
 *      Author: anton
 */

#include "types.h"
#include "leon.h"
#include "memory_map.h"
#include "cache.h"
#include "plug_and_play.h"

LEON_REGS * const l_regs = (PVOID) (RG_BASE);

int init() {
	irq_init_handlers();
	uart_init();

	timers_init();
	cache_data_enable(FALSE);
	cache_instr_enable(TRUE);
	return 0;
}

int main() {
	void* descriptor;
	init();
	print_ahb_dev();

	while (NULL != (descriptor = (void *) express_test_get_next())) {
		express_test_run(descriptor);
	}

	shell_start();

	while (1)
		;

	return 0;
}
