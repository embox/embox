/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for X86 architecture.
 *
 * @date 19.03.12
 * @author Gleb Efimov
 */


#include <embox/unit.h>

#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_core.h>

EMBOX_UNIT_INIT(mmu_init);

static int mmu_init(void) {
	return 0;
}

void mmu_on(void) {

	asm (
		"mov %cr0, %eax\n"
		"or  $0x8000000, %eax\n"
		"mov %eax, %cr0"
	);
}

void mmu_off(void) {
	asm (
		"mov %cr0, %eax\n"
		"and  $0x7fffffff, %eax\n"
		"mov %eax, %cr0"
	);
}
