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
#include "shell.h"

LEON_REGS * const l_regs = (PVOID) (RG_BASE);

void copy_data_section()
{
	extern char _data_image_start, _data_start, _data_end, _bstart, _bend;

	char *src = &_data_image_start;

	char *dst = &_data_start;

	/* ROM has data at end of text; copy it. */
	while (dst < &_data_end) {
	    *dst++ = *src++;
	}
	/* Zero bss */

	for (dst = &_bstart; dst< &_bend; dst++)
	    *dst = 0;
}
int init() {
	//TODO during too long time for simulation:(
	copy_data_section();

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


	while (NULL != (descriptor = (void *) express_test_get_next())) {
		express_test_run(descriptor);
	}

	shell_start();

	while (1)
		;

	return 0;
}
