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
#include <asm/flags.h>
#include <string.h>

//http://www.acm.uiuc.edu/sigops/roll_your_own/i386/vm.html
//http://citforum.ru/operating_systems/sos/glava_8.shtml
//http://nuclight.livejournal.com/129544.html


//http://elancev.narod.ru/texno/zr_vved/zr_vved.htm
//http://css.csail.mit.edu/6.858/2012/readings/i386/s04_01.htm

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
//http://tldp.org/HOWTO/Linux-i386-Boot-Code-HOWTO/kernel_head.html
static void set_cr0(unsigned int val)
{
   asm ("mov %0, %%cr0" : :"r" (val));
}


static unsigned int get_cr0(void)
{
   unsigned int _cr0;

   asm ("mov %%cr0, %0":"=r" (_cr0));
   return _cr0;
}

static unsigned int get_cr2(void) {
    unsigned int _cr2;

    asm ("mov %%cr2, %0":"=r" (_cr2):);
    return _cr2;
}

//static void set_tr6(unsigned int val)
//{
//   asm ("mov %0, %%tr6": :"r" (val));
//}
//
//static unsigned int get_tr7(void) {
//    unsigned int _tr7;
//
//    asm ("mov %%tr7, %0":"=r" (_tr7):);
//    return _tr7;
//}

mmu_pgd_t * mmu_get_root(mmu_ctx_t ctx) {
	mmu_pgd_t * pagedir;
	pagedir = (mmu_pgd_t *)get_cr3();
	return &pagedir[ctx];
}

uint32_t global_pdt[0x400] __attribute__((aligned(MMU_PAGE_SIZE)));
//#define global_pdt 0x600000
static int mmu_init(void) {
	uint32_t *pdt;
//	uint32_t tr7;
	extern char _text_vma, _data_vma, _stack_vma, _bss_vma;
	extern size_t _text_len, _data_len, _stack_len, _bss_len;

	pdt = (uint32_t *)global_pdt+0x1000;

	/* clear whole directory table */
	memset(pdt, 0, 0x1000);
	/* setup cr3 register (pointer to page directory table) */
	set_cr3((uint32_t)pdt);

	/* one-on-one mapping for context 0 */
	mmu_map_region(0, (paddr_t)&_text_vma, (vaddr_t)&_text_vma, (size_t)&_text_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(0, (paddr_t)&_data_vma, (paddr_t) &_data_vma, (size_t)&_data_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(0, (paddr_t)&_stack_vma, (paddr_t)&_stack_vma, (size_t)&_stack_len, MMU_PAGE_WRITEABLE);
	mmu_map_region(0, (paddr_t)&_bss_vma, (paddr_t)&_bss_vma, (size_t)&_bss_len, MMU_PAGE_WRITEABLE);

//	set_tr6((uint32_t)&_text_vma | (1 << 11) | 1 << 0);
//	tr7 = get_tr7();
//	set_tr6(tr7);


//	for (int i = 0; i < MMU_GTABLE_SIZE; i++) {
//		pte = pdt + (i + 1) * MMU_PAGE_SIZE;
//		pdt[i] = ((uint32_t) &pte) | 3;
//
//		for (int i = 0; i < MMU_MTABLE_SIZE; i++) {
//			pte[i] = address | 3;
//			address += MMU_PAGE_SIZE;
//		}
//	}


	return 0;
}

extern void setup_gp_flag(void);
void mmu_on(void) {
	setup_gp_flag();
}

void mmu_off(void) {
	set_cr0(get_cr0() & ~X86_CR0_PG);

}

vaddr_t mmu_get_fault_address(void) {
	return get_cr2();
}
