/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-18
 */

#include <inttypes.h>
#include <string.h>

#include <asm/hal/mmu.h>
#include <asm/regs.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/mmu.h>
#include <hal/reg.h>
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
	ARCH_REG_CLEAR(TTBCR, TTBCR_PAE_SUPPORT);
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

static void print_mmu_regs(void);

/**
* @brief Turn MMU on
*
* @note Set flag SCTLR_M at c1, the control register
*/
void mmu_on(void) {
	mmu_init();
#ifndef NOMMU
	__asm__ __volatile__("mrc p15, 0, r0, c1, c0, 0\n\t"
	                     "orr r0, r0, %[flag]\n\t" /* enabling MMU */
	                     "mcr p15, 0, r0, c1, c0, 0"
	                     :
	                     : [flag] "I"(SCTLR_M));
#endif

	print_mmu_regs();
}

/**
 * @brief Turn MMU off
 *
 * @note Clear flag SCTLR_M at c1, the control register
 */
void mmu_off(void) {
#ifndef NOMMU
	__asm__ __volatile__("mrc p15, 0, r0, c1, c0, 0\n\t"
	                     "bic r0, r0, %[flag]\n\t"
	                     "mcr p15, 0, r0, c1, c0, 0"
	                     :
	                     : [flag] "I"(SCTLR_M));
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
	uint32_t ttbr0 = ARCH_REG_LOAD(TTBR0);
	ttbr0 &= ~TTBR0_ADDR_MASK;
	ttbr0 |= ctx & TTBR0_ADDR_MASK;
	ARCH_REG_STORE(TTBR0, ttbr0);
	ARCH_REG_STORE(TTBR1, ttbr0);
}

#define CONTEXTIDR_ASID_MASK 0xFF
uint32_t arm_get_asid(void) {
	/* Suppose we always use short-descriptor format,
	 * so ASID is in the CONTEXTIDR register */
	return ARCH_REG_LOAD(CONTEXTIDR) & CONTEXTIDR_ASID_MASK;
}

void arm_set_asid(uint32_t asid) {
	/* Suppose we always use short-descriptor format,
	 * so ASID is in the CONTEXTIDR register */
	uint32_t contextidr = ARCH_REG_LOAD(CONTEXTIDR) & ~CONTEXTIDR_ASID_MASK;
	contextidr |= (asid & CONTEXTIDR_ASID_MASK);
	ARCH_REG_STORE(CONTEXTIDR, contextidr);
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

static void print_mmu_regs(void) {
#if LOG_LEVEL >= LOG_INFO
	/* Sometimes accessing this registers crushes the emulator */
	uint32_t fault_status;

	log_info("ARM MMU registers summary:");
	log_info("TLB Type:                  %#10x", ARCH_REG_LOAD(TLBTR));
	log_info("SCTLR:                     %#10x", ARCH_REG_LOAD(SCTLR));
	log_info("ACTLR:                     %#10x", ARCH_REG_LOAD(ACTLR));
	log_info("CPACR:                     %#10x", ARCH_REG_LOAD(CPACR));
	log_info("Non-Secure Access Control: %#10x", ARCH_REG_LOAD(NSACR));
	log_info("Translation Table Base 0:  %#10x", ARCH_REG_LOAD(TTBR0));
	log_info("Translation Table Base 1:  %#10x", ARCH_REG_LOAD(TTBR1));
	log_info("Domain Access Conrol:      %#10x", ARCH_REG_LOAD(DACR));

	fault_status = ARCH_REG_LOAD(DFSR);
	log_info("Data Fault Status:         %#10x", fault_status);
	if (fault_status) {
		log_info("Data Fault Address:        %#10x", ARCH_REG_LOAD(DFAR));
	}

	fault_status = ARCH_REG_LOAD(IFSR);
	log_info("Instruction Fault Status:  %#10x", fault_status);
	if (fault_status) {
		log_info("Instruction Fault Address: %#10x", ARCH_REG_LOAD(IFAR));
	}

	log_info("TLB lockdown:              %#10x", ARCH_REG_LOAD(TLBLR));
	log_info("Primary Region Remap:      %#10x", ARCH_REG_LOAD(PRRR));
	log_info("Normal Memory Remap:       %#10x", ARCH_REG_LOAD(NRRR));
	log_info("FCSE PID:                  %#10x", ARCH_REG_LOAD(FCSEIDR));
	log_info("Context ID:                %#10x", ARCH_REG_LOAD(CONTEXTIDR));
#ifdef CORTEX_A9
	/* CP15 c15 implemented */
	log_info("TLB Lockdown VA:           %#10x", ARCH_REG_LOAD(MTLBVAR));
	log_info("TLB Lockdown PA:           %#10x", ARCH_REG_LOAD(MTLBPAR));
	log_info("TLB Lockdown Attribues:    %#10x", ARCH_REG_LOAD(MTLBATR));
	/* CP15 c11, Reserved for TCM DMA registers */
	log_info("PLEIDR:                    %#10x", ARCH_REG_LOAD(PLEIDR));

	if (_get_pleidr()) {
		log_info("PLEASR:                    %#10x", ARCH_REG_LOAD(PLEASR));
		log_info("PLEFSR:                    %#10x", ARCH_REG_LOAD(PLEFSR));
		log_info("PLEUAR:                    %#10x", ARCH_REG_LOAD(PLEUAR));
		log_info("PLEPCR:                    %#10x", ARCH_REG_LOAD(PLEPCR));
	}
#endif /* CORTEX_A9 */
	log_info("MIDR:                      %#10x", ARCH_REG_LOAD(MIDR));
	log_info("CTR:                       %#10x", ARCH_REG_LOAD(CTR));
	log_info("TCMTR:                     %#10x", ARCH_REG_LOAD(TCMTR));
	log_info("TLBTR:                     %#10x", ARCH_REG_LOAD(TLBTR));
	log_info("MPIDR:                     %#10x", ARCH_REG_LOAD(MPIDR));
	log_info("REVIDR:                    %#10x", ARCH_REG_LOAD(REVIDR));
	log_info("PFR0:                      %#10x", ARCH_REG_LOAD(PFR0));
	log_info("PFR1:                      %#10x", ARCH_REG_LOAD(PFR1));
	log_info("DFR0:                      %#10x", ARCH_REG_LOAD(DFR0));
	log_info("AFR0:                      %#10x", ARCH_REG_LOAD(AFR0));
	log_info("MMFR0:                     %#10x", ARCH_REG_LOAD(MMFR0));
	log_info("MMFR1:                     %#10x", ARCH_REG_LOAD(MMFR1));
	log_info("MMFR2:                     %#10x", ARCH_REG_LOAD(MMFR2));
	log_info("MMFR3:                     %#10x", ARCH_REG_LOAD(MMFR3));
	log_info("ISAR0:                     %#10x", ARCH_REG_LOAD(ISAR0));
	log_info("ISAR1:                     %#10x", ARCH_REG_LOAD(ISAR1));
	log_info("ISAR2:                     %#10x", ARCH_REG_LOAD(ISAR2));
	log_info("ISAR3:                     %#10x", ARCH_REG_LOAD(ISAR3));
	log_info("ISAR4:                     %#10x", ARCH_REG_LOAD(ISAR4));
	log_info("CCSIDR:                    %#10x", ARCH_REG_LOAD(CCSIDR));
	log_info("CLIDR:                     %#10x", ARCH_REG_LOAD(CLIDR));
	log_info("AIDR:                      %#10x", ARCH_REG_LOAD(AIDR));
	log_info("CSSELR:                    %#10x", ARCH_REG_LOAD(CSSELR));
#endif
}
