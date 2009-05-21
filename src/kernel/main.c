/**
 * \file main.c
 *
 * \date Nov 27, 2008
 * \author anton
 */

#include "types.h"
#include "leon.h"
#include "memory_map.h"
#include "cache.h"
#include "plug_and_play.h"
#include "shell.h"

LEON_REGS * const l_regs = (PVOID) (RG_BASE);


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
