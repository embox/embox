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

LEON_REGS *const l_regs        = (PVOID)(RG_BASE);

typedef int (*EXPRESS_TEST_FUNC)(void);
typedef struct _EXPRESS_TEST_DESCRIPTION
{
	char *name;
	EXPRESS_TEST_FUNC handler;
}EXPRESS_TEST_DESCRIPTION;
EXPRESS_TEST_DESCRIPTION express_tests={

};
EXPRESS_TEST_DESCRIPTION *express_test_get_next();
int express_test_run (EXPRESS_TEST_DESCRIPTION *test_handler);
int init()
{
	irq_init_handlers();

	uart_init();

	con_init ();
	timers_init();
	cache_data_enable (FALSE);
	cache_instr_enable (TRUE);
	return 0;
}

int main()
{
	init();
	printf ("hello");
	while(1);
	return 0;
}

