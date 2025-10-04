/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    17.07.2024
 */

#include <stdint.h>
#include <string.h>

#include <lib/libcpu_info.h>

static struct cpu_info current_cpu;

struct cpu_info *get_cpu_info(void) {
	memset(&current_cpu, 0, sizeof(current_cpu));
	strcpy(current_cpu.vendor_id, "NIISI RAS");

    set_feature_strval(&current_cpu, "Chip name", "K5500VK018");
    set_feature_strval(&current_cpu, "CPU revision", "0x0513050c (VM8)");
    set_feature_strval(&current_cpu, "Primary inst cache", "16kB");
    set_feature_strval(&current_cpu, "Primary data cache", "16kB");
    set_feature_strval(&current_cpu, "Secondary data cache", "128kB");
	//strcpy(current_cpu.feature[0].name, "CPU revision");
	//current_cpu.feature[0].val = 0x0513050c;
	return &current_cpu;
}

uint64_t get_cpu_counter(void) {
	return 0;
}
