/**
 * @file
 * @brief Test work of mmu
 *
 * @date 19.03.12
 * @author Gleb Efimov
 */

#include <stdio.h>
#include <embox/cmd.h>

/* #include <hal/mm/mmu_core.h> */

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	printf("Inside command mmu, mmu switch to on: \n\n");
	asm (
			"mov $0x00000000, %eax\n"
			"mov %eax, %cr0"
		);
	printf("Processor is in protected mode. Go back in real mode: \n\n");
	asm (
			"mov $0x00000001, %eax\n"
			"mov %eax, %cr0"
		);
	printf("Processor is in real mode.");
	return 0;
}
