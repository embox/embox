/**
 * \file main.c
 *
 * \date Nov 27, 2008
 * \author anton
 */

#include "types.h"
#include "amba_pnp.h"
#include "shell.h"

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
