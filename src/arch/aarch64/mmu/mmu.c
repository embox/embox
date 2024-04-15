/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.08.2019
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <hal/mem_barriers.h>
#include <hal/mmu.h>
#include <hal/reg.h>
#include <mem/vmem.h>
#include <util/field.h>
#include <util/log.h>

#include "mmu.h"

#define DEFAULT_ASID        0

#define MEM_DEVICE_ATTRINDX 0
#define MEM_NORMAL_ATTRINDX 1

static int mmu_init(void) {
	uint64_t mair;
	uint64_t tcr;
	uint64_t tmp;

	/* Load Translation Control Register */
	tcr = ARCH_REG_LOAD(TCR_EL1);

	switch (AARCH64_MMU_GRANULE) {
	case 4:
		tmp = TCR_ELn_TG0_4KB;
		break;
	case 16:
		tmp = TCR_ELn_TG0_16KB;
		break;
	case 64:
		tmp = TCR_ELn_TG0_64KB;
		break;
	default:
		tmp = TCR_ELn_TG0_4KB;
		log_crit("Wrong granule configuration '%i'", AARCH64_MMU_GRANULE);
	}

	/* Set Granule Size */
	tcr = FIELD_SET(tcr, TCR_ELn_TG0, tmp);

	/* Set maximum PA range */
	tmp = ARCH_REG_LOAD(ID_AA64MMFR0_EL1);
	tmp = FIELD_GET(tmp, ID_AA64MMFR0_EL1_PAR);
	tcr = FIELD_SET(tcr, TCR_EL1_IPS, tmp);

	/* Set 48-bit VA range 0 (0x0000_0000_0000_0000 - 0x0000_FFFF_FFFF_FFFF) */
	tcr = FIELD_SET(tcr, TCR_ELn_T0SZ, 64 - 48);

	/* Set 48-bit VA range 1 (0xFFFF_0000_0000_0000 - 0xFFFF_FFFF_FFFF_FFFF) */
	tcr = FIELD_SET(tcr, TCR_ELn_T1SZ, 64 - 48);

	/* TTBR0_EL1.ASID defines the ASID */
	tcr &= ~TCR_EL1_A1;

	/* Store Translation Control Register */
	ARCH_REG_STORE(TCR_EL1, tcr);

	/* Set memory attributes (0 - device; 1 - normal) */
	mair = ARCH_REG_LOAD(MAIR_EL1);
	mair = FIELD_SET(mair, MAIR_ELn_ATTR0, MAIR_ELn_ATTRn_DEVICE_nGnRnE);
	mair = FIELD_SET(mair, MAIR_ELn_ATTR1, MAIR_ELn_ATTRn_NORMAL);
	ARCH_REG_STORE(MAIR_EL1, mair);

	return 0;
}

void mmu_on(void) {
	static bool initialized = false;

	if (!initialized) {
		mmu_init();
		initialized = true;
	}

	dmb(sy);

	/* Enable MMU (stage 1 address translation) */
	ARCH_REG_ORIN(SCTLR_EL1, SCTLR_ELn_M);
}

void mmu_off(void) {
	/* Flush TLB */
	ARCH_REG_STORE(TLBI_VMALLE1, 0);

	/* Disable MMU (stage 1 address translation) */
	ARCH_REG_CLEAR(SCTLR_EL1, SCTLR_ELn_M);
}

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
	mmu_ctx_t ctx;

	ctx = (mmu_ctx_t)pgd;

	/* Assume mmu_ctx_t and ttbr0 are the same values for aarch64, so just set
	   ASID=0 for now (as we have a single address space system-wide) */
	if (DEFAULT_ASID != FIELD_GET(ctx, TTBRn_EL1_ASID)) {
		log_crit("16 most-sign bits of pgd should be zero (pgd=%p)", pgd);
		ctx = FIELD_SET(ctx, TTBRn_EL1_ASID, DEFAULT_ASID);
		return 0;
	}

	return ctx;
}

void mmu_set_context(mmu_ctx_t ctx) {
	/* Set base address of translation table 0 */
	ARCH_REG_STORE(TTBR0_EL1, ctx);

	/* Set base address of translation table 1 */
	ARCH_REG_STORE(TTBR1_EL1, ctx);
}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
	return (uintptr_t *)FIELD_GET(ctx, TTBRn_ELn_BADDR);
}

void mmu_flush_tlb(void) {
	ARCH_REG_STORE(TLBI_VMALLE1, 0);
}

mmu_vaddr_t mmu_get_fault_address(void) {
	return ARCH_REG_LOAD(FAR_EL1);
}

uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	if (!entry) {
		log_error("Entry is NULL!");
		return 0;
	}

	return (uintptr_t *)(*entry & ~MMU_PAGE_MASK);
}

void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	if (!entry) {
		log_error("entry is NULL!");
		return;
	}

	if ((lvl < 0) || (lvl > MMU_LAST_LEVEL)) {
		log_error("wrong MMU level: %i (max %i)", lvl, MMU_LAST_LEVEL);
		return;
	}

	*entry = value | MMU_DESC_VD | MMU_DESC_TP;
}

void mmu_unset(int lvl, uintptr_t *entry) {
	if (!entry) {
		log_error("entry is NULL!");
		return;
	}

	if ((lvl < 0) || (lvl > MMU_LAST_LEVEL)) {
		log_error("wrong MMU level: %i (max %i)", lvl, MMU_LAST_LEVEL);
		return;
	}

	*entry = 0;
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
	uintptr_t pte;

	if (addr & MMU_PAGE_MASK) {
		log_error("address %p doesn't match mask %p", (void *)addr,
		    (void *)MMU_PAGE_MASK);
		addr &= MMU_PAGE_MASK;
	}

	pte = addr | MMU_DESC_VD | MMU_DESC_TP | MMU_DESC_AF;

	if (prot & PROT_WRITE) {
		pte = FIELD_SET(pte, MMU_DESC_AP, MMU_DESC_AP_RW1);
	}
	else {
		pte = FIELD_SET(pte, MMU_DESC_AP, MMU_DESC_AP_RO1);
	}

	if (!(prot & PROT_READ)) {
		log_error("Setting page non-readable WTF");
	}

	if (prot & PROT_NOCACHE) {
		/* Use memory attribute 0 (device) */
		pte = FIELD_SET(pte, MMU_DESC_ATTRINDX, MEM_DEVICE_ATTRINDX);
	}
	else {
		/* Use memory attribute 1 (normal) */
		pte = FIELD_SET(pte, MMU_DESC_ATTRINDX, MEM_NORMAL_ATTRINDX);
	}

	if (!(prot & PROT_EXEC)) {
		pte |= MMU_DESC_PXN;
	}

	return pte;
}

int mmu_pte_set(uintptr_t *entry, uintptr_t value) {
	assert(entry);

	*entry = value;

	return 0;
}

uintptr_t mmu_pte_get(uintptr_t *entry) {
	if (entry == NULL) {
		log_error("Entry is NULL!");
		return 0;
	}

	return *entry;
}

int mmu_present(int lvl, uintptr_t *entry) {
	if (entry == NULL) {
		log_error("Entry is NULL!");
		return 0;
	}

	return !!(*entry & MMU_DESC_VD);
}

uintptr_t mmu_pte_unpack(uintptr_t pte, int *flags) {
	int tmp;

	assert(flags);

	if (!(pte & MMU_DESC_VD) || !(pte & MMU_DESC_TP)) {
		log_error("Trying to unpack corrupted PTE");
	}

	*flags = 0;

	if (!(pte & MMU_DESC_PXN)) {
		*flags |= PROT_EXEC;
	}

	tmp = FIELD_GET(pte, MMU_DESC_AP);
	switch (tmp) {
	case MMU_DESC_AP_RW1:
		*flags |= PROT_READ | PROT_WRITE;
		break;
	case MMU_DESC_AP_RO1:
		*flags |= PROT_READ;
		break;
	default:
		log_error("Corrupted PTE access properties");
	}

	tmp = FIELD_GET(pte, MMU_DESC_ATTRINDX);
	switch (tmp) {
	case MEM_DEVICE_ATTRINDX:
		*flags |= PROT_NOCACHE;
		break;
	case MEM_NORMAL_ATTRINDX:
		break;
	default:
		log_error("Corrupted PTE cache properties");
	}

	return pte & ~MMU_PAGE_MASK;
}
