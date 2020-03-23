/*
 *@brief CPU_info implementation
 *
 *@date 24.10.2013
 *
 *
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <lib/libcpu_info.h>

struct cpu_info current_cpu;

static void set_cpu_features(struct cpu_info *info) {
	uint32_t reg; /* register: edx */

	asm volatile (	"movl $0x1, %%eax   \n\t"
					"cpuid				\n\t"
					"movl %%edx, %0		\n\t"
					: "=g"(reg)
					:
					: "%eax", "%ebx", "%ecx", "%edx");
	
	set_feature_val(info, "FPU", reg & 0x1);
	set_feature_val(info, "MMU", (reg >> 23) & 0x1);
	set_feature_val(info, "SSE", (reg >> 25) & 0x1);
	return;
}

static void set_cpu_freq(struct cpu_info *info) {
	uint32_t reg[2]; /* registers: eax, ebx */

	asm volatile (	"movl $0x16, %%eax   \n\t"
					"cpuid				\n\t"
					"movl %%eax, %0		\n\t"
					"movl %%ebx, %1		\n\t"
					: "=g"(reg[0]), "=g"(reg[1])
					:
					: "%eax", "%ebx", "%ecx", "%edx");
	
	set_feature_val(info, "Base Freq", reg[0]);
	set_feature_val(info, "Max Freq", reg[1]);
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
	current_cpu.feature_count = 0;
	set_vendor_id(&current_cpu);
	set_cpu_freq(&current_cpu);
	set_cpu_features(&current_cpu);
	return &current_cpu;
}
