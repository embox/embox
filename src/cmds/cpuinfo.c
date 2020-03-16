/**
 * @file
 * @brief Displays CPU info.
 *
 * @date 23.10.2013
 * @author Denis Deryugin
 */

#include <unistd.h>
#include <stdio.h>
#include <lib/libcpu_info.h>

int main(int argc, char **argv) {
	struct cpu_info *info = get_cpu_info();
	printf("%-20s %s\n", "CPU Vendor ID ", info->vendor_id);
	
	for(int i = 0; i < info->feature_count; i++) {
		printf("CPU %-16s %u\n", info->feature[i].name, info->feature[i].val);
	}
	
	printf("Current time stamp counter: %llu\n", get_cpu_counter());
	return 0;
}
