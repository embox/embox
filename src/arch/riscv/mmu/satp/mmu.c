#include "mmu.h"
#include <asm/csr.h>
#include <mem/vmem.h>
#include <hal/mmu.h>

static uintptr_t *ctx_table[0x100] __attribute__((aligned(MMU_PAGE_SIZE)));

static int ctx_counter = 0;

void mmu_on(void) {
    uint64_t satp_val = (read_csr(satp) | ((uint64_t)SATP_MODE << 60)); // Enable MMU for SV48
    write_csr(satp, satp_val);
    asm volatile ("sfence.vma" : : : "memory"); // Flush TLB after enabling MMU
}

void mmu_off(void) {
    uintptr_t satp_val = 0;  // Disable paging
    write_csr(satp, satp_val);
    asm volatile ("sfence.vma" : : : "memory"); // Flush TLB after disabling MMU
}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
    return ctx_table[ctx];
}

int mmu_present(int lvl, uintptr_t *entry)  {
    return (uint64_t)*entry & MMU_PAGE_VALID;
}

void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
    *entry = (value | MMU_INIT_FLAGS);
}

void mmu_unset(int lvl, uintptr_t *entry) {
    *entry = 0;
}

uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
    return (uintptr_t *) (*entry & MMU_PTE_PMASK);
}

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
    mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
    ctx_table[ctx] = pgd;
    return ctx;
}

void mmu_set_context(mmu_ctx_t ctx) {
    write_csr(satp, ((read_csr(satp) & (~SATP_PPN_MASK)) | (uintptr_t) mmu_get_root(ctx)));
    asm volatile ("sfence.vma" : : : "memory"); // Flush TLB
}

int mmu_pte_set(uintptr_t *entry, uintptr_t value) {
        *entry = value;
        return 0;
}

uintptr_t mmu_pte_get(uintptr_t *entry) {
	return *entry;
}

void mmu_flush_tlb(void) {
    asm volatile("sfence.vma zero, zero" : : : "memory");
}

void mmu_flush_tlb_single(unsigned long addr) {
    asm volatile("sfence.vma %0, zero" : : "r"(addr) : "memory");
}

mmu_vaddr_t mmu_get_fault_address(void) {
    return read_csr(stval); // Returns the fault address from stval
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
    return addr | (prot & MMU_FLAG_MASK);
}

uintptr_t mmu_pte_unpack(uintptr_t pte, int *flags) {
    *flags = pte & MMU_FLAG_MASK;
    return pte & (~MMU_PAGE_MASK);
}
