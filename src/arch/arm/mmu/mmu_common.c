/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-18
 */

#include <inttypes.h>
#include <string.h>

#include <asm/cp15.h>
#include <asm/hal/mmu.h>
#include <asm/regs.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/mmu.h>
#include <kernel/printk.h>
#include <mem/vmem.h>
#include <util/log.h>

//EMBOX_UNIT_INIT(mmu_init);

#define DOMAIN_ACCESS OPTION_GET(NUMBER, domain_access)
#define CTX_NUMBER    32 /* TODO: make it related to number of tasks */
#define LOG_LEVEL     OPTION_GET(STRING, log_level)

#define KERNEL_ASID   0
void arm_set_asid(uint32_t asid);

void arm_set_contextidr(uint32_t val);

static void arm_disable_pae_support(void) {
	cp15_set_ttbcr(cp15_get_ttbcr() & ~TTBCR_PAE_SUPPORT);
}

/**
 * @brief Fill translation table and so on
 * @note Assume MMU is off right now
 *
 * @return
 */
static int mmu_init(void) {
	arm_disable_pae_support();

	__asm__ __volatile__(
	/* setup c3, Domain Access Control Register */
#if DOMAIN_ACCESS == 1
	    "mov r0, #0x55\n\t" /* Client for all domains */
#elif DOMAIN_ACCESS == 3
	    "mov r0, #0xff\n\t" /* Manager for all domains */
#else
#error Given domain access level is not supported
#endif
	    "orr r0, r0, lsl #8\n\t"
	    "orr r0, r0, lsl #16\n\t"
	    "mcr p15, 0, r0, c3, c0, 0\n\t"
	    :
	    :);

	arm_set_asid(KERNEL_ASID);

	return 0;
}

void _print_mmu_regs(void);

/**
* @brief Turn MMU on
*
* @note Set flag CR_M at c1, the control register
*/
void mmu_on(void) {
	mmu_init();
#ifndef NOMMU
	__asm__ __volatile__("mrc p15, 0, r0, c1, c0, 0\n\t"
	                     "orr r0, r0, %[flag]\n\t" /* enabling MMU */
	                     "mcr p15, 0, r0, c1, c0, 0"
	                     :
	                     : [flag] "I"(CR_M));
#endif

	_print_mmu_regs();
}

/**
 * @brief Turn MMU off
 *
 * @note Clear flag CR_M at c1, the control register
 */
void mmu_off(void) {
#ifndef NOMMU
	__asm__ __volatile__("mrc p15, 0, r0, c1, c0, 0\n\t"
	                     "bic r0, r0, %[flag]\n\t"
	                     "mcr p15, 0, r0, c1, c0, 0"
	                     :
	                     : [flag] "I"(CR_M));
#endif
}

void mmu_flush_tlb(void) {
	uint32_t zero = 0;

	__asm__ __volatile__("mcr p15, 0, %[zero], c8, c7, 0"
	                     :
	                     : [zero] "r"(zero)
	                     :);
}

mmu_vaddr_t mmu_get_fault_address(void) {
	mmu_vaddr_t val;

	__asm__ __volatile__("mrc p15, 0, %[out], c6, c0, 0" : [out] "=r"(val) :);

	return val;
}

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
	return (mmu_ctx_t)pgd;
}

void mmu_set_context(mmu_ctx_t ctx) {
	log_info("mmu set ctx 0x%" PRIx32, (uint32_t)ctx);
	uint32_t ttbr0 = cp15_get_ttbr0();
	ttbr0 &= ~TTBR0_ADDR_MASK;
	ttbr0 |= ctx & TTBR0_ADDR_MASK;
	cp15_set_ttbr0(ttbr0);
	cp15_set_ttbr1(ttbr0);
}

void arm_set_contextidr(uint32_t val);

#define CONTEXTIDR_ASID_MASK 0xFF
uint32_t arm_get_asid(void) {
	/* Suppose we always use short-descriptor format,
	 * so ASID is in the CONTEXTIDR register */
	return cp15_get_contextidr() & CONTEXTIDR_ASID_MASK;
}

void arm_set_asid(uint32_t asid) {
	/* Suppose we always use short-descriptor format,
	 * so ASID is in the CONTEXTIDR register */
	uint32_t contextidr = cp15_get_contextidr() & ~CONTEXTIDR_ASID_MASK;
	contextidr |= (asid & CONTEXTIDR_ASID_MASK);

	return arm_set_contextidr(contextidr);
}

/**
 * @brief Get address of first translation level
 * @note XXX We have the same ctx for all tasks
 *
 * @param ctx
 *
 * @return Pointer to translation table
 */
uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
	return (uintptr_t *)ctx;
}

void _print_mmu_regs(void) {
#if LOG_LEVEL >= LOG_INFO
	/* Sometimes accessing this registers crushes the emulator */
	uint32_t fault_status;

	log_info("ARM MMU registers summary:");
	log_info("TLB Type:                  %#10x", cp15_get_mmu_tlb_type());
	log_info("SCTRL:                     %#10x", cp15_get_sctrl());
	log_info("ACTRL:                     %#10x", cp15_get_actrl());
	log_info("CPACR:                     %#10x", cp15_get_cpacr());
	log_info("Non-Secure Access Control: %#10x", cp15_get_nsacr());
	log_info("Translation Table Base 0:  %#10x",
	    cp15_get_translation_table_base_0());
	log_info("Translation Table Base 1:  %#10x",
	    cp15_get_translation_table_base_1());
	log_info("Domain Access Conrol:      %#10x",
	    cp15_get_domain_access_control());

	fault_status = cp15_get_data_fault_status();
	log_info("Data Fault Status:         %#10x", fault_status);
	if (fault_status) {
		log_info("Data Fault Address:        %#10x",
		    cp15_get_data_fault_address());
	}

	fault_status = cp15_get_instruction_fault_status();
	log_info("Instruction Fault Status:  %#10x", fault_status);
	if (fault_status) {
		log_info("Instruction Fault Address: %#10x",
		    cp15_get_instruction_fault_address());
	}

	log_info("TLB lockdown:              %#10x", cp15_get_tlb_lockdown());

	log_info("Primary Region Remap:      %#10x",
	    cp15_get_primary_region_remap());
	log_info("Normal Memory Remap:       %#10x",
	    cp15_get_normal_memory_remap());

	log_info("FSCE PID:                  %#10x", cp15_get_fsce_pid());

	log_info("Context ID:                %#10x", cp15_get_contextidr());
#ifdef CORTEX_A9
	/* CP15 c15 implemented */
	log_info("Peripheral port remap:     %#10x",
	    cp15_get_mmu_peripheral_port_memory_remap());

	log_info("TLB Lockdown Index:        %#10x",
	    cp15_get_tlb_lockdown_index());
	log_info("TLB Lockdown VA:           %#10x", cp15_get_tlb_lockdown_va());
	log_info("TLB Lockdown PA:           %#10x", cp15_get_tlb_lockdown_pa());
	log_info("TLB Lockdown Attribues:    %#10x",
	    cp15_get_tlb_lockdown_attributes());
	/* CP15 c11, Reserved for TCM DMA registers */
	log_info("PLEIDR:                    %#10x", cp15_get_pleidr());

	if (_get_pleidr()) {
		log_info("PLEASR:                    %#10x", cp15_get_pleasr());
		log_info("PLESFR:                    %#10x", cp15_get_plesfr());
		log_info("PLEAUR:                    %#10x", cp15_get_pleuar());
		log_info("PLEPCR:                    %#10x", cp15_get_plepcr());
	}
#endif /* CORTEX_A9 */
	log_info("MIDR:                      %#10x", cp15_get_midr());
	log_info("CTR:                       %#10x", cp15_get_ctr());
	log_info("TCMTR:                     %#10x", cp15_get_tcmtr());
	log_info("TLBTR:                     %#10x", cp15_get_tlbtr());
	log_info("MPIDR:                     %#10x", cp15_get_mpidr());
	log_info("REVIDR:                    %#10x", cp15_get_revidr());

	log_info("PFR0:                      %#10x", cp15_get_pfr0());
	log_info("PFR1:                      %#10x", cp15_get_pfr1());
	log_info("DFR0:                      %#10x", cp15_get_dfr0());
	log_info("AFR0:                      %#10x", cp15_get_afr0());

	log_info("MMFR0:                     %#10x", cp15_get_mmfr0());
	log_info("MMFR1:                     %#10x", cp15_get_mmfr1());
	log_info("MMFR2:                     %#10x", cp15_get_mmfr2());
	log_info("MMFR3:                     %#10x", cp15_get_mmfr3());

	log_info("ISAR0:                     %#10x", cp15_get_isar0());
	log_info("ISAR1:                     %#10x", cp15_get_isar1());
	log_info("ISAR2:                     %#10x", cp15_get_isar2());
	log_info("ISAR3:                     %#10x", cp15_get_isar3());
	log_info("ISAR4:                     %#10x", cp15_get_isar4());

	log_info("CCSIDR:                    %#10x", cp15_get_ccsidr());
	log_info("CLIDR:                     %#10x", cp15_get_clidr());
	log_info("AIDR:                      %#10x", cp15_get_aidr());
	log_info("CSSELR:                    %#10x", cp15_get_csselr());
#endif
}
