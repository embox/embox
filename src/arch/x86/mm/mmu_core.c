/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for X86 architecture.
 *
 * @date 19.03.12
 * @author Gleb Efimov
 */

#include <hal/mm/mmu_core.h>

void mmu_on(void) {

	asm (
		"mov %cr0, %eax\n"
		"or  $0x00000001, %eax\n"
		"mov %eax, %cr0"
	);
}

void mmu_off(void) {
	asm (
		"mov %cr0, %eax\n"
		"and  $0xfffffffe, %eax\n"
		"mov %eax, %cr0"
	);
}
