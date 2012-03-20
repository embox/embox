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
	__asm__ (
		"mov $0x00000001, %eax"
		"mov %eax, %cr0"
	);
}

void mmu_off(void) {
	__asm__ (
		"mov $0x00000000, %eax"
		"mov %eax, %cr0"
	);
}
