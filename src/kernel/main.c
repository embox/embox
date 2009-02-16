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

int scanf_test()
{
	int d;
	char s[10];
	char c;

	printf("Enter decimal number... ");
	scanf ("%d", &d);
	printf("\nYou have entered %d\n\n", d);

	printf("Enter oct number... ");
	scanf ("%o", &d);
	printf("\nYou have entered x%o\n\n", d);

	printf("Enter hex number... ");
	scanf ("%x", &d);
	printf("\nYou have entered 0x%x\n\n", d);

	printf("Enter string... ");
	scanf("%10s", s);
	printf("String of first ten symbols is: %s\n\n", s);

	printf("Enter char...");
	scanf("%c", &c);
	printf("Char you have entered is: %c\n\n", c);

	int o;
	printf("Scan string: One know  31 oct == 25 dec\n");
	scanf("%o %d", &o , &d);
	printf("We see %d == %d. Its true.\n", o , d);
}

int main() {
	void* descriptor;
	init();
	print_all_pnp_devs();

	scanf_test();

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
