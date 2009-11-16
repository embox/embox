/**
 * \file main.c
 *
 * \date Nov 27, 2008
 * \author anton
 */
#include "types.h"
#include "asm/cpu_context.h"
#include "shell.h"
#include "conio.h"
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
#if !defined(SIMULATION_TRG) && defined(MONITOR_SHELL)
#ifdef MONITOR_NETWORK
	net_config();
#endif /* MONITOR_NETWORK */
	shell_start();
#endif

	while (1)
		;

	return 0;
}
