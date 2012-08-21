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

//http://www.acm.uiuc.edu/sigops/roll_your_own/i386/vm.html

EMBOX_UNIT_INIT(mmu_init);

static void set_cr3(unsigned int pagedir)
{
   asm ("mov %0, %%cr3": :"r" (pagedir));
}

static unsigned int get_cr3(void)
{
   unsigned int _cr3;

   asm ("mov %%cr3, %0":"=r" (_cr3));
   return _cr3;
}

static unsigned int get_cr2(void) {
    unsigned int _cr2;

    asm ("mov %%cr2, %0":"=r" (_cr2):);
    return _cr2;
}

mmu_pgd_t * mmu_get_root(mmu_ctx_t ctx) {
	mmu_pgd_t * pagedir;
	pagedir = (mmu_pgd_t *)get_cr3();
	return &pagedir[ctx];
}

#define START 0x5C0000

static int mmu_init(void) {
	uint32_t address = 0x0;
	uint32_t *pdt = (uint32_t *) START;
	uint32_t *pte;

	for (int i = 0; i < MMU_GTABLE_SIZE; i++) {
		pte = pdt + (i + 1) * MMU_PAGE_SIZE;
		pdt[i] = ((uint32_t) &pte) | 3;

		for (int i = 0; i < MMU_MTABLE_SIZE; i++) {
			pte[i] = address | 3;
			address += MMU_PAGE_SIZE;
		}
	}

	pdt[0] = (uint32_t) pte;
	pdt[0] |= 3;

	set_cr3((uint32_t)pdt);
	return 0;
}

void mmu_on(void) {

	asm (
			"mov %cr0, %eax\n"
			"or  $0x80000000, %eax\n"
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

vaddr_t mmu_get_fault_address(void) {
	return get_cr2();
}
