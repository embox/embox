/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    17.07.2024
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <hal/cpu.h>
#include <lib/libcpu_info.h>

static struct cpu_info current_cpu;

struct cpu_info *get_cpu_info(void) {
	uint32_t hartid;
	char buf[20];
	char *str;

	memset(&current_cpu, 0, sizeof(current_cpu));
	strcpy(current_cpu.vendor_id, "MILANDR");

	hartid = 217;

	set_feature_strval(&current_cpu, "Chip name", "MDR1206FI");
	set_feature_strval(&current_cpu, "CPU ARCH", "RISC-V");

	str = itoa(hartid, buf, 10);
	str = buf;
	set_feature_strval(&current_cpu, "CPU ID:", str);

	return &current_cpu;
}

uint64_t get_cpu_counter(void) {
	return 0;
}
