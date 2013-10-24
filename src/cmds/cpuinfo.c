/**
 * @file
 * @brief Displays CPU info.
 *
 * @date 23.10.2013
 * @author Denis Deryugin
 */

#include <embox/cmd.h>

#include <hal/cpu_counter.h>
#include <unistd.h>
#include <stdio.h>

EMBOX_CMD(exec);

static char vendor_id[13];

static void get_vendor_id(void) {
	uint64_t r[3]; /* registers: ebx, ecx, edx */
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
			vendor_id[i * 4 + j] = (r[i] >> (8 * j)) % 256;
		}
	}
	vendor_id[12] = 0;
	return;
}

static int exec(int argc, char **argv) {
	get_vendor_id();
	printf("CPU Vendor ID:              %s\n", vendor_id);
	printf("Current time stamp counter: %llu\n", get_cpu_counter());
	return 0;
}
