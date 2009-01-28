/*
 * test_cpu_context.c
 *
 *  Created on: Jan 26, 2009
 *      Author: anton
 */
#include "types.h"
#include "common.h"
#include "test_cpu_context.h"

int flag_restored_context = 0;


int test_cpu_context ()
{
	asm ("ta 0");
	printf ("save context done\n");

	if (!flag_restored_context)
	{
		flag_restored_context ++;
		printf ("restore context start\n");
		asm ("ta 8");
		printf ("restore context faild\n");
	}
	else
	{
		printf ("restore context done\n");
	}
	return 0;
}
