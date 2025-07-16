/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    17.07.2024
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <asm/regs.h>
#include <lib/libcpu_info.h>

static struct cpu_info current_cpu;

struct cpu_info *get_cpu_info(void) {
	uint32_t hartid;
	char buf[20];
	char *str;

	memset(&current_cpu, 0, sizeof(current_cpu));
	strcpy(current_cpu.vendor_id, "MIKRON");

#if !SMODE
	hartid = read_csr(mhartid);
#else
	asm volatile("add %0, tp, x0" : "=r" (hartid) );
#endif

	set_feature_strval(&current_cpu, "Chip name", "AMUR");
	set_feature_strval(&current_cpu, "CPU ARCH", "RISC-V");

	buf[0] = '0';
	buf[1] = 'x';
	str = itoa((hartid >> 28) & 0xF, buf+2, 16);
	str = buf;
	set_feature_strval(&current_cpu, "CPU revision:", str);
	str = itoa((hartid >> 16) & 0xFFF, buf+2, 16);
	str = buf;
	set_feature_strval(&current_cpu, "CPU part:", str);

	return &current_cpu;
}

uint64_t get_cpu_counter(void) {
	return 0;
}
