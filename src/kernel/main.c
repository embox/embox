/**
 * \file main.c
 *
 * \date Nov 27, 2008
 * \author anton
 */

#include "amba_pnp.h"
#include "shell.h"
#include "common.h"
#include "express_tests.h"

int main() {

#ifdef MONITOR_TESTS
	if (0 != express_tests_execute()) {
		TRACE("express tests fault\n halt system\n");
		sys_halt();
		while (1)
			;
	}
#endif //MONITOR_TESTS
#if !defined(SIMULATION_TRG) && defined(MONITOR_CONIO)
	net_config();
	shell_start();
#endif

	while (1)
		;

	return 0;
}
