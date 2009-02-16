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

void sscanf_test()
{
	int d;
	char s[35] = "We have wrote it February, 16";
	char subs[7];
	char c;


	printf("String for parse: %s",s);

	sscanf( s, "%d", &d );
	printf("Dec number in string is %d", d);

	sscanf( s, "%x", &d );
	printf("Hex number in string is %x", d);

	sscanf( s, "%c", &c );
	printf("Char in string is %c", c);

	sscanf( s, "%7s", subs );
	printf("And 7-length string is %s", subs);

}

int main() {
	void* descriptor;
	init();
	print_all_pnp_devs();

	sscanf_test();

	while (1)
		;
//	while (NULL != (descriptor = (void *) express_test_get_next())) {
//		express_test_run(descriptor);
//	}

	shell_start();

	while (1)
		;

	return 0;
}
