/**
 * \file main.c
 *
 * \date Nov 27, 2008
 * \author anton
 */

#include "types.h"
#include "amba_pnp.h"
#include "shell.h"
#include "common.h"

int main() {

	init();

#ifdef MONITOR_TESTS_EXEC
	if (0 != express_tests_execute()){
	    printf ("express tests fault\n halt system\n");
	    sys_halt();
	    while (1)
	         ;
	}
#endif //MONITOR_TESTS_EXEC

#ifndef SIMULATION_TRG
	shell_start();
#endif
	while (1)
		;

	return 0;
}
