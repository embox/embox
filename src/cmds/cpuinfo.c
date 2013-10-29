/**
 * @file
 * @brief Displays CPU info.
 *
 * @date 23.10.2013
 * @author Denis Deryugin
 */

#include <embox/cmd.h>

#include <hal/cpu_info.h>
#include <unistd.h>
#include <stdio.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	struct cpu_info *info = get_cpu_info();
	printf("CPU Vendor ID:              %s\n", info->vendor_id);
	printf("Current time stamp counter: %llu\n", get_cpu_counter());
	return 0;
}
