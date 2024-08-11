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
#include <hal/cpu.h>

struct cpu_info current_cpu;
static uint32_t max_cpuid_leaf;

static uint32_t check_max_cpuid_leaf(void) {
	uint32_t max_leaf;

	asm volatile (	"xorl %%eax, %%eax	\n\t"
					"cpuid				\n\t"
					"movl %%eax, %0		\n\t"
					: "=a" (max_leaf)
					: );
	return max_leaf;
}

static void set_cpu_features(struct cpu_info *info) {
	uint32_t reg; /* register: edx */

	/* 0x1 is initial EAX number */
	if(max_cpuid_leaf < 0x1){
		set_feature_strval(info, "FPU", "NaN");
		set_feature_strval(info, "MMU", "NaN");
		set_feature_strval(info, "SSE", "NaN");
		return;
	} else {
		asm volatile (	"movl $0x1, %%eax   \n\t"
						"cpuid				\n\t"
						"movl %%edx, %0		\n\t"
						: "=d"(reg)
						: );

		set_feature_val(info, "FPU", reg & 0x1);
		set_feature_val(info, "MMU", (reg >> 23) & 0x1);
		set_feature_val(info, "SSE", (reg >> 25) & 0x1);
		return;
	}
}

static void set_cpu_freq(struct cpu_info *info) {
	uint32_t reg[2]; /* registers: eax, ebx */

	/* 0x16 is initial EAX number */
	if(max_cpuid_leaf < 0x16){
		set_feature_strval(info, "Base Freq", "NaN");
		set_feature_strval(info, "Max Freq", "NaN");
		return;
	} else {
		asm volatile (	"movl $0x16, %%eax   \n\t"
						"cpuid				\n\t"
						"movl %%eax, %0		\n\t"
						"movl %%ebx, %1		\n\t"
						: "=a"(reg[0]), "=b"(reg[1])
						: );

		set_feature_val(info, "Base Freq", reg[0]);
		set_feature_val(info, "Max Freq", reg[1]);
		return;
	}
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
					: );
	*/

	/* Getting vendor id */
	/* Leaf 0x0 is supported in all CPUs, No need to test max_cpuid_leaf */

	asm volatile (	"xorl %%eax, %%eax	\n\t"
					"cpuid				\n\t"
					"movl %%ebx, %0		\n\t"
					"movl %%ecx, %1		\n\t"
					: "=b"(r[0]), "=d"(r[1]), "=c"(r[2])
					: );

	/* Parsing registers */
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 4; j++) {
			info->vendor_id[i * 4 + j] = (r[i] >> (8 * j)) % 256;
		}
	}
	info->vendor_id[12] = 0;
	return;
}

static void set_cpu_number(struct cpu_info *info) {
	set_feature_val(info, "CPU NUM", NCPU);
	return;
}

struct cpu_info* get_cpu_info(void) {
	current_cpu.feature_count = 0;
	max_cpuid_leaf = check_max_cpuid_leaf();
	set_vendor_id(&current_cpu);
	set_cpu_freq(&current_cpu);
	set_cpu_features(&current_cpu);
	set_cpu_number(&current_cpu);
	return &current_cpu;
}
