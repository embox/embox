/**
 * @file
 * @brief
 *
 * @date 04.10.2012
 * @author Anton Bulychev
 */

#include <hal/mmu.h>
#include <asm/flags.h>
#include <types.h>

static mmu_pgd_t *ctx_table[0x100] __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_counter = 0;

static inline void set_cr3(unsigned int pagedir)
{
   asm ("mov %0, %%cr3": :"r" (pagedir));
}

static inline unsigned int get_cr3(void)
{
   unsigned int _cr3;

   asm ("mov %%cr3, %0":"=r" (_cr3));
   return _cr3;
}

static inline void set_cr0(unsigned int val)
{
   asm ("mov %0, %%cr0" : :"r" (val));
}


static inline unsigned int get_cr0(void)
{
   unsigned int _cr0;

   asm ("mov %%cr0, %0":"=r" (_cr0));
   return _cr0;
}

static inline unsigned int get_cr2(void) {
    unsigned int _cr2;

    asm ("mov %%cr2, %0":"=r" (_cr2):);
    return _cr2;
}

void mmu_flush_tlb_single(unsigned long addr) {
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

void mmu_flush_tlb(void) {
	set_cr3(get_cr3());
}

void mmu_on(void) {
    set_cr0(get_cr0() | X86_CR0_PG);
}

void mmu_off(void) {
	set_cr0(get_cr0() & ~X86_CR0_PG);
}

mmu_vaddr_t mmu_get_fault_address(void) {
	return get_cr2();
}

void switch_mm(mmu_ctx_t prev, mmu_ctx_t next) {
	set_cr3((uint32_t) mmu_get_root(next));
}

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (ctx_counter++);
	ctx_table[ctx] = pgd;
	return ctx;
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return ctx_table[ctx];
}

mmu_pmd_t *mmu_pgd_follow(mmu_pgd_t *pgd) {
	return (mmu_pmd_t *) ((*pgd) & (~MMU_PAGE_MASK));
}

mmu_pte_t *mmu_pmd_follow(mmu_pmd_t *pmd) {
	return (mmu_pte_t *) pmd;
}

mmu_paddr_t mmu_pte_follow(mmu_pte_t *pte) {
	return (mmu_paddr_t) ((*pte) & (~MMU_PAGE_MASK));
}

