/**
 * @file
 * @brief RISC-V MMU Implementation
 *
 * @date 04.09.2024
 * @author Suraj Sonawane
 */

#include "mmu.h"
#include <asm/regs.h> 
#include <mem/vmem.h>  
#include <util/log.h>

#include <stdint.h>

#include <hal/mmu.h>
#include <mem/vmem.h>

#define MMU_PTE_FLAG    (MMU_PAGE_WRITABLE | MMU_PAGE_USERMODE)

static uintptr_t *ctx_table[0x100] __attribute__((aligned(MMU_PAGE_SIZE)));

static int ctx_counter = 0;

void mmu_on(void) {
    uint64_t satp_val = (read_csr(satp) | ((uint64_t)SATP_MODE_SV39 << 60)); // Enable MMU for SV39
    write_csr(satp, satp_val);
    asm volatile ("sfence.vma" : : : "memory"); // Flush TLB after enabling MMU
}

void mmu_off(void) {
    uintptr_t satp_val = 0;  // Disable paging
    write_csr(satp, satp_val);
    asm volatile ("sfence.vma" : : : "memory"); // Flush TLB after disabling MMU
}
                   
void mmu_flush_tlb_single(unsigned long addr) {
    asm volatile("sfence.vma %0, zero" : : "r"(addr) : "memory");
}

void mmu_flush_tlb(void) {
    asm volatile("sfence.vma zero, zero" : : : "memory");
}

mmu_vaddr_t mmu_get_fault_address(void) {
    return read_csr(stval); // Returns the fault address from stval
}

void mmu_set_context(mmu_ctx_t ctx) {
    write_csr(satp, ((read_csr(satp) & ~0xFFFFFFFFFFUL) | (uintptr_t) mmu_get_root(ctx)));
    asm volatile ("sfence.vma" : : : "memory"); // Flush TLB
}

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
    mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
    ctx_table[ctx] = pgd;
    return ctx;
}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
    return ctx_table[ctx];
}

int mmu_present(int lvl, uintptr_t *entry)  {
	switch(lvl) {
	case 0:
		return (uint32_t)*entry & MMU_PAGE_PRESENT;
	case 1:
		return (uint32_t)*entry & MMU_PAGE_PRESENT;
	}
	return 0;
}

void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
    *entry = (value | MMU_PTE_FLAG | MMU_PAGE_PRESENT);
}


uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
    return (uintptr_t *) (*entry & MMU_PTE_PMASK);
}

void mmu_unset(int lvl, uintptr_t *entry) {
    *entry = 0;
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
    int flags = 0;
    if (prot & PROT_WRITE) {
        flags |= MMU_PAGE_WRITABLE;
    }
    if (prot & PROT_EXEC) {
        flags |= MMU_PAGE_EXECUTABLE;
    }
    if (prot & VMEM_PAGE_USERMODE) {
        flags |= MMU_PAGE_USERMODE;
    }
    return addr | flags | MMU_PAGE_PRESENT;
}

int mmu_pte_set(uintptr_t *entry, uintptr_t value) {
	*entry = value;
	return 0;
}

uintptr_t mmu_pte_get(uintptr_t *entry) {
	return *entry;
}

uintptr_t mmu_pte_unpack(uintptr_t pte, int *flags) {
    int prot = 0;
    if (pte & MMU_PAGE_WRITABLE) {
        prot |= PROT_WRITE;
    }
    if (pte & MMU_PAGE_EXECUTABLE) {
        prot |= PROT_EXEC;
    }
    *flags = prot;
    return pte & MMU_PAGE_MASK;
}
