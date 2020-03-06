/**
 * @file
 * @brief Displays CPU info.
 *
 * @date 23.10.2013
 * @author Denis Deryugin
 */

#include <hal/cpu_info.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
	struct cpu_info *info = get_cpu_info();
	printf("CPU Vendor ID:              %s\n", info->vendor_id);
	printf("CPU Base Frequency (MHz):   %u\n", info->base_freq);
	printf("CPU Max Frequency (MHz):    %u\n", info->max_freq);
	printf("CPU FPU Support:            %s\n", info->FPU == 1 ? "true" : "false");
	printf("CPU MMX Support:            %s\n", info->MMX == 1 ? "true" : "false");
	printf("CPU SSE Support:            %s\n", info->SSE == 1 ? "true" : "false");
	printf("Current time stamp counter: %llu\n", get_cpu_counter());
	return 0;
}
