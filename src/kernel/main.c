/**
 * \file main.c
 *
 * \date Nov 27, 2008
 * \author anton
 */
#include "shell.h"
#include "autoconf.h"

int main() {
#if !defined(SIMULATION_TRG) && defined(MONITOR_SHELL)
	shell_start();
#endif

	while (1)
		;

	return 0;
}
