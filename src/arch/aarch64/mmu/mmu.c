/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.08.2019
 */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <hal/mmu.h>
#include <hal/reg.h>
#include <hal/test/traps_core.h>
#include <mem/vmem.h>
#include <util/field.h>
#include <util/log.h>

static int aarch64_mmu_init(void) {
	uint64_t tcr;
	uint64_t tcr_tg0;
	uint64_t mmfr0;
	int current_el;

	current_el = FIELD_GET(ARCH_REG_LOAD(CurrentEL), CurrentEL_EL);

	switch (AARCH64_MMU_GRANULE) {
	case 4:
		tcr_tg0 = TCR_ELn_TG0_4KB;
		break;
	case 16:
		tcr_tg0 = TCR_ELn_TG0_16KB;
		break;
	case 64:
		tcr_tg0 = TCR_ELn_TG0_64KB;
		break;
	default:
		tcr_tg0 = TCR_ELn_TG0_4KB;
		log_crit("mmu: Wrong granule configuration");
	}

	switch (current_el) {
	case 1:
		tcr = ARCH_REG_LOAD(TCR_EL1);
		/* Set Granule Size */
		tcr = FIELD_SET(tcr, TCR_ELn_TG0, tcr_tg0);
		/* Set maximum PA range */
		mmfr0 = ARCH_REG_LOAD(ID_AA64MMFR0_EL1);
		tcr = FIELD_SET(tcr, TCR_EL1_IPS,
		    (FIELD_GET(mmfr0, ID_AA64MMFR0_EL1_PAR)));
		/* Set 48bit VA range */
		tcr = FIELD_SET(tcr, TCR_ELn_T0SZ, 64 - 48);
		ARCH_REG_STORE(TCR_EL1, tcr);
		break;
	case 2:
		tcr = ARCH_REG_LOAD(TCR_EL2);
		/* Set Granule Size */
		tcr = FIELD_SET(tcr, TCR_ELn_TG0, tcr_tg0);
		ARCH_REG_STORE(TCR_EL2, tcr);
		break;
	default:
		log_crit("mmu: EL%i not supported", current_el);
	}

	return 0;
}

void mmu_on(void) {
	int current_el;

	current_el = FIELD_GET(ARCH_REG_LOAD(CurrentEL), CurrentEL_EL);

	aarch64_mmu_init();

	switch (current_el) {
	case 1:
		ARCH_REG_ORIN(SCTLR_EL1, SCTLR_ELn_M);
		break;
	case 2:
		ARCH_REG_ORIN(SCTLR_EL2, SCTLR_ELn_M);
		break;
	default:
		log_crit("mmu: EL%i not supported", current_el);
	}
}

void mmu_off(void) {
	int current_el;

	current_el = FIELD_GET(ARCH_REG_LOAD(CurrentEL), CurrentEL_EL);

	/* TODO: flush tlb */

	switch (current_el) {
	case 1:
		ARCH_REG_CLEAR(SCTLR_EL1, SCTLR_ELn_M);
		break;
	case 2:
		ARCH_REG_CLEAR(SCTLR_EL2, SCTLR_ELn_M);
		break;
	default:
		log_crit("mmu: EL%i not supported", current_el);
	}
}

/* Assume mmu_ctx_t and ttbr0 are the same values for aarch64, so
 * just set ASID=0 for now (as we have a single address space system-wide) */
#define DEFAULT_ASID 0LL
#define ASID_OFFSET  48
#define ASID_MASK    (0xFFFFLL << ASID_OFFSET)
mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
	if ((uintptr_t)pgd & ASID_MASK) {
		log_error("16 most-significant bits of pgd should be zero, but we "
		          "have: %p",
		    pgd);
		return 0;
	}

	return ((uintptr_t)pgd) | (DEFAULT_ASID << ASID_OFFSET);
}

void mmu_set_context(mmu_ctx_t ctx) {
	int current_el;

	current_el = FIELD_GET(ARCH_REG_LOAD(CurrentEL), CurrentEL_EL);

	switch (current_el) {
	case 1:
		ARCH_REG_STORE(TTBR0_EL1, ctx);
		break;
	case 2:
		ARCH_REG_STORE(TTBR0_EL2, ctx);
		break;
	default:
		log_crit("mmu: EL%i not supported", current_el);
	}
}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
	return (uintptr_t *)(ctx & ~ASID_MASK);
}

void mmu_flush_tlb(void) {
	int current_el;

	current_el = FIELD_GET(ARCH_REG_LOAD(CurrentEL), CurrentEL_EL);

	switch (current_el) {
	case 1:
		ARCH_REG_STORE(TLBI_VMALLE1, 0);
		break;
	case 2:
		ARCH_REG_STORE(TLBI_ALLE2, 0);
		break;
	default:
		log_crit("mmu: EL%i not supported", current_el);
	}
}

mmu_vaddr_t mmu_get_fault_address(void) {
	int current_el;

	current_el = FIELD_GET(ARCH_REG_LOAD(CurrentEL), CurrentEL_EL);

	switch (current_el) {
	case 1:
		return ARCH_REG_LOAD(FAR_EL1);
	case 2:
		return ARCH_REG_LOAD(FAR_EL2);
	default:
		log_crit("mmu: EL%i not supported", current_el);
	}

	return 0;
}

uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	if (entry == NULL) {
		log_error("Entry is NULL!");
		return 0;
	}

	return (uintptr_t *)(*entry & ~MMU_PAGE_MASK);
}

void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	if (entry == 0) {
		log_error("entry is NULL!");
		return;
	}

	if (lvl < 0 || lvl >= MMU_LEVELS) {
		log_error("Wrong MMU level: %d "
		          "(should be in range [0; %d)",
		    lvl, MMU_LEVELS);
		return;
	}

	if (lvl == MMU_LEVELS - 1) {
		*entry = value | AARCH64_MMU_TYPE_TABLE;
	}
	else {
		*entry = value | AARCH64_MMU_TYPE_PAGE;
	}
}

void mmu_unset(int lvl, uintptr_t *entry) {
	if (entry == 0) {
		log_error("entry is NULL!");
		return;
	}

	if (lvl < 0 || lvl >= MMU_LEVELS) {
		log_error("Wrong MMU level: %d "
		          "(should be in range [0; %d)",
		    lvl, MMU_LEVELS);
		return;
	}

	*entry = 0;
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
	uintptr_t ret;

	if (addr & MMU_PAGE_MASK) {
		log_error("address %p doesn't match mask %p", (void *)addr,
		    (void *)MMU_PAGE_MASK);
		addr &= MMU_PAGE_MASK;
	}

	ret = addr | AARCH64_MMU_TYPE_PAGE | AARCH64_MMU_PROPERTY_AF;

	if (prot & PROT_WRITE) {
		ret |= AARCH64_MMU_PROPERTY_AP_RW;
	}
	else {
		ret |= AARCH64_MMU_PROPERTY_AP_RO;
	}

	if (!(prot & PROT_READ)) {
		log_error("Setting page non-readable WTF");
	}

	if (prot & PROT_NOCACHE) {
		ret |= AARCH64_MMU_PROPERTY_MEM_ATTR_DEVICE;
	}
	else {
		ret |= AARCH64_MMU_PROPERTY_MEM_ATTR_NORMAL;
	}

	if (!(prot & PROT_EXEC)) {
		ret |= AARCH64_MMU_PROPERTY_UXN | AARCH64_MMU_PROPERTY_PXN;
	}

	return ret;
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

	return !!(*entry & AARCH64_MMU_TYPE_MASK);
}

uintptr_t mmu_pte_unpack(uintptr_t pte, int *flags) {
	assert(flags);

	if ((pte & AARCH64_MMU_TYPE_MASK) != AARCH64_MMU_TYPE_PAGE) {
		log_warning("Trying to unpack corrupted PTE");
	}

	*flags = 0;

	if (!(pte & (AARCH64_MMU_PROPERTY_UXN | AARCH64_MMU_PROPERTY_PXN))) {
		*flags |= PROT_EXEC;
	}

	switch (pte & AARCH64_MMU_PROPERTY_AP_MASK) {
	case AARCH64_MMU_PROPERTY_AP_RW:
		*flags |= PROT_READ | PROT_WRITE;
		break;
	case AARCH64_MMU_PROPERTY_AP_RO:
		*flags |= PROT_READ;
		break;
	default:
		log_warning("Corrupted PTE access properties");
	}

	switch (pte & AARCH64_MMU_PROPERTY_MEM_ATTR_MASK) {
	case AARCH64_MMU_PROPERTY_MEM_ATTR_DEVICE:
		*flags |= PROT_NOCACHE;
		break;
	case AARCH64_MMU_PROPERTY_MEM_ATTR_NORMAL:
		/* Do nothing */
		break;
	default:
		log_warning("Corrupted PTE cache properties");
	}

	return pte & ~MMU_PAGE_MASK;
}

void set_fault_handler(enum fault_type type, fault_handler_t handler) {
	log_error("%s is not implemented yet for Aarch64", __func__);
}

/* TODO it's impossible to simply drop priveleges as it
 * requires using `eret` instruction, and this can be
 * a little bit complicated */
void mmu_drop_privileges(void) {
	log_error("%s is not implemented yet for Aarch64", __func__);
}

void mmu_sys_privileges(void) {
	log_error("%s is not implemented yet for Aarch64", __func__);
}
