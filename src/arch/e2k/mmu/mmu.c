/**
 * @file
 *
 * @date Apr 26, 2019
 * author: Anton Bondarev
 */
#include <util/log.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <mem/vmem.h>
#include <mmu_regs.h>
#include <hal/mmu.h>

static uintptr_t context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_cnt;

void mmu_on(void) {
	mmu_reg_t mmu_cr;
	WRITE_MMU_ELB_PTB(KERNEL_VMLPT_BASE_ADDR);
	WRITE_MMU_CONT(0);

/* Flush hardware stacks and wait for all memory operations to finish */
	E2K_FLUSHCPU;
	E2K_WAIT_ALL;

	mmu_cr = READ_MMU_CR();
	mmu_cr = _MMU_CR_TLB_EN | _MMU_CR_SET3;
	E2K_WAIT_ALL;
	WRITE_MMU_CR(mmu_cr);
	E2K_WAIT_ALL;

	log_error("READ_MMU_ROOT_PTB() %lx", READ_MMU_ROOT_PTB());
}

void mmu_off(void) {

}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
	return (uintptr_t *)context_table[0];
}

uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
	case 1:
	case 2:
	case 3:
		return (uintptr_t *)(((*entry) & E2K_MMU_PAGE_PFN));
	}
	return 0;
}

void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	uintptr_t flags = 0;

	flags |= E2K_MMU_PAGE_PV;
	flags |= E2K_MMU_PAGE_VALID;
	//flags |= E2K_MMU_PAGE_W;
	flags |= E2K_MMU_PAGE_D | E2K_MMU_PAGE_A;
	//flags |= E2K_MMU_PAGE_G;

	switch (lvl) {
	case 0:
	case 1:
	case 2:
	case 3:
		*entry = (value & E2K_MMU_PAGE_PFN) | flags | E2K_MMU_PAGE_P | E2K_MMU_PAGE_W ;
		return;
	}
}

void mmu_unset(int lvl, uintptr_t *entry) {
	switch(lvl) {
	case 1:
	case 2:
	case 3:
		*entry = 0;
		break;
	}
}

int mmu_present(int lvl, uintptr_t *entry) {
	switch(lvl) {
	case 0:
	case 1:
	case 2:
	case 3:
		return *entry & E2K_MMU_PAGE_P;
	}
	return 0;
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
	uintptr_t flags = 0;

	if (prot & PROT_WRITE) {
		flags |= E2K_MMU_PAGE_W;
	}
	if (prot & PROT_NOCACHE) {
		flags |= (E2K_MMU_PAGE_CD1 | E2K_MMU_PAGE_CD2 | E2K_MMU_PAGE_PWT);
	}
	flags |= E2K_MMU_PAGE_PV;
	flags |= E2K_MMU_PAGE_VALID;
	flags |= E2K_MMU_PAGE_W;
	flags |= E2K_MMU_PAGE_A;

	return addr | flags | E2K_MMU_PAGE_P | E2K_MMU_PAGE_G;
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

	if (pte & E2K_MMU_PAGE_W) {
		prot |= PROT_WRITE;
	}
	if (pte & (E2K_MMU_PAGE_CD1 | E2K_MMU_PAGE_CD2)) {
		prot |= PROT_NOCACHE;
	}

	*flags = prot;

	return pte & E2K_MMU_PAGE_PFN;
}

void mmu_flush_tlb(void) {
	E2K_WAIT(_st_c);
	E2K_WRITE_MAS_D(_flush_op_tlb_all, 0UL, MAS_TLB_FLUSH);
	E2K_WAIT(_fl_c | _ma_c);
}

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_cnt);

	context_table[0] = ((uintptr_t)(pgd) & E2K_MMU_PAGE_PFN);

	return ctx;
}

void mmu_set_context(mmu_ctx_t ctx) {
	uintptr_t pgd = context_table[0];

	WRITE_MMU_ROOT_PTB(pgd);
}
