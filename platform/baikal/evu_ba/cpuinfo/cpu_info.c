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
	strcpy(current_cpu.vendor_id, "Baikal Electronics");

	hartid = cpu_get_id();

	set_feature_strval(&current_cpu, "Chip name", "BE-U1000");
	set_feature_strval(&current_cpu, "CPU ARCH", "RISC-V");

	buf[0] = '0';
	buf[1] = 'x';
	str = itoa((hartid) , buf + 2, 16);
	str = buf;
	set_feature_strval(&current_cpu, "CPU ID:", str);

	return &current_cpu;
}

uint64_t get_cpu_counter(void) {
	return 0;
}
