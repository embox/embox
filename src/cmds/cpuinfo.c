/**
 * @file
 * @brief Displays CPU info.
 *
 * @date 23.10.2013
 * @author Denis Deryugin
 */

#include <embox/cmd.h>

#include <hal/cpu_counter.h>
#include <unistd.h>
#include <stdio.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	printf("Current time stamp counter: %llu\n", get_cpu_counter());
	return 0;
}
