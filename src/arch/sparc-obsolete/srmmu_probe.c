/**
 * \file srmmu_probe.c
 * \date Jun 18, 2009
 * \author anton
 * \details
 */

#include "common.h"
#include "asm/mmu.h"

#define NODO_CLEAR
int mmu_probe_init(void) {
    ctxd_t *c0 = (ctxd_t *) &ctx;
    pgd_t *g0 = (pgd_t *) &pg0;
    //alloc mem for tables
    //ctx context table
    __asm__(
            ".section .data\n\t"
            ".align %0\n\t"
            "ctx: .skip %1\n\t"
            ".align %1\n\t"
            "pg0: .skip %1\n\t"
            ".align %2\n\t"
            "pm0: .skip %2\n\t"
            ".align %3\n\t"
            "pt0: .skip %3\n\t"
            ".text\n"
            : : "i" (PAGE_SIZE),
            "i"(SRMMU_PGD_TABLE_SIZE) ,
            "i"(SRMMU_PMD_TABLE_SIZE) ,
            "i"(SRMMU_PTE_TABLE_SIZE) );

    /* Prepare Page Table Hirarchy */
#ifndef NODO_CLEAR
    /* use ram vhl model that clear mem at startup to suppress this loop */
    for (i = 0; i < SRMMU_PTRS_PER_CTX; i++) {
        srmmu_ctxd_set(c0 + i, (pgd_t *) 0);
    }
#endif /*DO_CLEAR*/
    srmmu_set_ctable_ptr((unsigned long) c0);
    /* one-on-one mapping for context 0 */
    //paddr = 0;
    srmmu_ctxd_set(c0 + 0, (pgd_t *) g0);
    //printf ("set c0 = 0x%X\n", c0);
    //printf ("set g0 = 0x%X\n", g0);


    return 0;
}

void mmu_probe_set_pgd(uint32_t addr) {

}

#define MMU_GTABLE_MASK         0xFF000000
#define MMU_GTABLE_MASK_OFFSET  24
#define MMU_MTABLE_MASK         0x00FC0000
#define MMU_MTABLE_MASK_OFFSET  18
#define MMU_PTABLE_MASK         0x0003F000
#define MMU_PTABLE_MASK_OFFSET  12
#define MMU_PAGE_MASK           0xFFF

int mmu_probe_map_region(uint32_t paddr, uint32_t vaddr, uint32_t size, uint32_t flags) {

    pgd_t *g0 = (pgd_t *) &pg0;
    pmd_t *m0 = (pmd_t *) &pm0;
    pte_t *p0 = (pte_t *) &pt0;
    unsigned long pteval;
    //align on page size
    size &= ~MMU_PAGE_MASK;
    paddr &= ~MMU_PAGE_MASK;
    vaddr &= ~MMU_PAGE_MASK;
    if (flags)
        flags |= SRMMU_ET_PTE;
    if (0 == (size & (~MMU_GTABLE_MASK))) {//alloc 16Mb
        //g0 must set in mmu_probe_init
        paddr &= MMU_GTABLE_MASK;
        vaddr &= MMU_GTABLE_MASK;
        srmmu_set_pte(g0 + (vaddr >> MMU_GTABLE_MASK_OFFSET), (paddr >> 4)
                | flags);

        return 0;
    }
    if (0 == (size & (~(MMU_GTABLE_MASK | MMU_MTABLE_MASK)))) {//alloc 256kb
        if (0 == *(g0
                + ((vaddr & MMU_GTABLE_MASK) >> (MMU_GTABLE_MASK_OFFSET )))) {//if middle table not set
            srmmu_pgd_set(g0 + ((vaddr & MMU_GTABLE_MASK)
                    >> MMU_GTABLE_MASK_OFFSET), m0);
        }
        //paddr &= MMU_MTABLE_MASK;
        vaddr &= MMU_MTABLE_MASK;
        srmmu_set_pte(m0 + (vaddr >> MMU_MTABLE_MASK_OFFSET), (paddr >> 4)
                | flags);
        return 0;
    }
    if (0 == (size & ~0xFFFFF000)) {//alloc 4kb
        if (0 == *(g0
                + ((vaddr & MMU_GTABLE_MASK) >> (MMU_GTABLE_MASK_OFFSET )))) {//if middle table not set
            srmmu_pgd_set(g0 + ((vaddr & MMU_GTABLE_MASK)
                    >> MMU_GTABLE_MASK_OFFSET), m0);
        }
        if (0 == *(m0 + ((vaddr & MMU_MTABLE_MASK) >> MMU_MTABLE_MASK_OFFSET))) {
            srmmu_pmd_set(m0 + ((vaddr & MMU_MTABLE_MASK)
                    >> MMU_MTABLE_MASK_OFFSET), p0);
        }
        //paddr &= MMU_PTABLE_MASK;
        vaddr &= MMU_PTABLE_MASK;
        srmmu_set_pte(p0 + (vaddr >> MMU_PTABLE_MASK_OFFSET), (paddr >> 4)
                | flags);
        return 0;
    }

    TRACE("mmu brobe set region error\n");
    return -1;
}

unsigned int mmu_get_page_desc(unsigned int page_addr) {
    pte_t *p0 = (pte_t *) &pt0;
    int i;
    for (i = 0; i < 0x40; i ++){
        if ((page_addr >> 4) == (p0[i] & 0xFFFFFF00)){
            return p0[i];
        }
    }
    return 0;
}

int mmu_probe_start(void) {
    srmmu_set_context(0);
    srmmu_set_mmureg(0x00000001);
    __asm__(" flush ");
    //iflush
    __asm__(" sta  %g0, [%g0] 0x11 ");
    //dflush

    return 0;
}

void flush_data_cache(void) {
    __asm__(" sta  %g0, [%g0] 0x11 ");
    //dflush
}

void mmu_probe_repair_table_init(unsigned long page0_addr) {
    pgd_t *g0 = (pgd_t *) &pg0;
    pmd_t *m0 = (pmd_t *) &pm0;
    pte_t *p0 = (pte_t *) &pt0;
    unsigned long *pthaddr = &pth_addr1;
    //repair table
    *((unsigned long **) &pth_addr) = pthaddr;
    /* repair info for fault (0xf1000000)*/
    pthaddr[0] = (unsigned long) (g0 + 241);
    pthaddr[1] = ((0x40000000 >> 4) | SRMMU_ET_PTE | SRMMU_PRIV);
    pthaddr[2] = 0xf1000000;
    /* repair info for write protection fault (0xf0041000) */
    pthaddr[3] = (unsigned long) (p0 + 1);
    pthaddr[4] = (((page0_addr) >> 4) | SRMMU_ET_PTE | SRMMU_PRIV);
    pthaddr[5] = 0xf0041000;
    /* repair info for instruction page fault (0xf0042000) */
    pthaddr[6] = (unsigned long) (p0 + 2);
    pthaddr[7] = ((((unsigned long) mmu_func1) >> 4) | SRMMU_ET_PTE
            | SRMMU_PRIV);
    pthaddr[8] = 0xf0042000;
    /* repair info for priviledge protection fault (0xf0041000) */
    pthaddr[9] = (unsigned long) (p0 + 1);
    pthaddr[10] = ((page0_addr >> 4) | SRMMU_ET_PTE | SRMMU_EXEC | SRMMU_WRITE);
    pthaddr[11] = 0xf0041000;

}

void leon_flush_cache_all(void) {
    __asm__ __volatile__(" flush ");
    __asm__ __volatile__("sta %%g0, [%%g0] %0\n\t": :
            "i" (0x11) : "memory");

}

void leon_flush_tlb_all(void) {
    leon_flush_cache_all();
    __asm__ __volatile__("sta %%g0, [%0] %1\n\t": :
            "r" (0x400),
            "i" (0x18) : "memory");
}

