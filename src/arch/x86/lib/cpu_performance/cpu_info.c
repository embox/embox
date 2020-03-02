/*
 *@brief CPU_info implementation
 *
 *@date 24.10.2013
 *
 *
 */

#include <stdint.h>

#include <hal/cpu_info.h>

struct cpu_info current_cpu;

static void set_cpu_freq(struct cpu_info *info) {
	return;
}

static void set_vendor_id(struct cpu_info *info) {
	uint32_t r[3]; /* registers: ebx, ecx, edx */
	int i, j;

	/* Check if CPU supports CPUID instruction */
	/*asm volatile (	"pushfq\n\t"
					"popq %%rbp\n\t"
					"mov %%eax, %%ebx\n\t"
					"xorl $0x200000, %%eax\n\t"
					"push %%eax\n\t"
					"popf\n\t"
					"pushfq\n\t"
					"popq %%eax\n\t"
					"xorl %%ebx, %%eax\n\t"
					"movl %%eax, %0"
					: "=r"(a)
					: );*/

	/* Getting vendor id */
	/* For some mysterious reason I can't include all three registers
	 * EBX, ECX, EDX to clobber list, so I have to call CPUID twice.
	 */
	asm volatile (	"xorl %%eax, %%eax	\n\t"
					"cpuid				\n\t"
					"movl %%ebx, %0		\n\t"
					"movl %%ecx, %1		\n\t"
					: "=g"(r[0]), "=g"(r[2])
					:
					: "%ebx", "%ecx");

	asm volatile (	"xorl %%eax, %%eax	\n\t"
					"cpuid				\n\t"
					"movl %%edx, %0		\n\t"
					: "=g"(r[1])
					:
					: "%edx");

	/* Parsing registers */
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 4; j++) {
			info->vendor_id[i * 4 + j] = (r[i] >> (8 * j)) % 256;
		}
	}
	info->vendor_id[12] = 0;
	return;
}


struct cpu_info* get_cpu_info(void) {
	set_vendor_id(&current_cpu);
	set_cpu_freq(&current_cpu);
	return &current_cpu;
}
