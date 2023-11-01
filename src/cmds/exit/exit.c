/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.06.2014
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hal/arch.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>

int main(int argc, char *argv[]) {
	char *endptr;
	int ecode;

	if (task_self() == task_kernel_task()) {
		arch_shutdown(ARCH_SHUTDOWN_MODE_REBOOT);
	}

	ecode = 0;

	if (argc > 1) {
		ecode = strtol(argv[1], &endptr, 0);
		if (*endptr != '\0') {
			fprintf(stderr, "exit: exit code should be numerical value\n");
			ecode = 255;
		}
	}

	exit(ecode);
}
