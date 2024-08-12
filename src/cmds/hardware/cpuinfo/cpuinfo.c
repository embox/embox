/**
 * @file
 * @brief Displays CPU info.
 *
 * @date 23.10.2013
 * @author Denis Deryugin
 */

#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>

#include <lib/libcpu_info.h>

int main(int argc, char **argv) {
	uint64_t cpu_cnt;
	struct cpu_info *info;

	info = get_cpu_info();

	printf("%-20s %s\n", "CPU Vendor ID ", info->vendor_id);
	
	for(int i = 0; i < info->feature_count; i++) {
		cpu_feature_print(&info->feature[i]);
		printf("\n");
	}

	cpu_cnt = get_cpu_counter();
	if (cpu_cnt) {
		printf("Current time stamp counter: %" PRId64 "\n", cpu_cnt);
	}
	return 0;
}
