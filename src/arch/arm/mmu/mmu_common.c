/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-18
 */

#include <string.h>

#include <asm/hal/mmu.h>
#include <asm/regs.h>
#include <embox/unit.h>
#include <hal/mmu.h>
#include <mem/vmem.h>
#include <util/log.h>

#include <framework/mod/options.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(mmu_init);

#define DOMAIN_ACCESS OPTION_GET(NUMBER, domain_access)
#define CTX_NUMBER    32 /* TODO: make it related to number of tasks */
#define LOG_LEVEL     OPTION_GET(NUMBER, log_level)
/**
 * @brief Fill translation table and so on
 * @note Assume MMU is off right now
 *
 * @return
 */
static int mmu_init(void) {
	__asm__ __volatile__ (
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
		: :
	);

	return 0;
}

void _print_mmu_regs(void);

/**
* @brief Turn MMU on
*
* @note Set flag CR_M at c1, the control register
*/
void mmu_on(void) {
#ifndef NOMMU
	__asm__ __volatile__ (
		"mrc p15, 0, r0, c1, c0, 0\n\t"
		"orr r0, r0, %[flag]\n\t" /* enabling MMU */
		"mcr p15, 0, r0, c1, c0, 0"
		: : [flag] "I" (CR_M)
	);
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
	__asm__ __volatile__ (
		"mrc p15, 0, r0, c1, c0, 0\n\t"
		"bic r0, r0, %[flag]\n\t"
		"mcr p15, 0, r0, c1, c0, 0"
		: : [flag] "I" (CR_M)
	);
#endif
}

void mmu_flush_tlb(void) {
	uint32_t zero = 0;

	__asm__ __volatile__ (
			"mcr p15, 0, %[zero], c8, c7, 0" : : [zero] "r" (zero) :
	);
}

mmu_vaddr_t mmu_get_fault_address(void) {
	mmu_vaddr_t val;

	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c6, c0, 0" : [out] "=r" (val) :
	);

	return val;
}

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	return (mmu_ctx_t) pgd;
}

void mmu_set_context(mmu_ctx_t ctx) {
	__asm__ __volatile__ (
		"mcr p15, 0, %[addr], c2, c0, 0\n\t"
		: : [addr] "r" (ctx) :
	);
}

/**
 * @brief Get address of first translation level
 * @note XXX We have the same ctx for all tasks
 *
 * @param ctx
 *
 * @return Pointer to translation table
 */
mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return (void*) ctx;
}

/* Software accessible MMU registers */
uint32_t _get_mmu_tlb_type(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_sctrl_control(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c1, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_actrl_control(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c1, c0, 1" : [out] "=r" (val) :
	);
	return val;
}
/* Coprocessor Access Control Register */
uint32_t _get_cpacr_control(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c1, c0, 2" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_nonsecure_access_control(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c1, c1, 2" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_translation_table_base_0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c2, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_translation_table_base_1(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c2, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_domain_access_control(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c3, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_data_fault_status(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c5, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_instruction_fault_status(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c5, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_data_fault_address(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c6, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_instruction_fault_address(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c6, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_tlb_lockdown(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c10, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_primary_region_remap(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c10, c2, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_normal_memory_remap(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c10, c2, 1" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_fsce_pid(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c13, c2, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_context_id(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c13, c2, 1" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_peripheral_port_memory_remap(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c2, 4" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_tlb_lockdown_index(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c4, 2" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_tlb_lockdown_va(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c5, 2" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_tlb_lockdown_pa(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c6, 2" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_mmu_tlb_lockdown_attributes(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c7, 2" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_pleidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_pleasr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c0, 2" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_plesfr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c0, 4" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_pleuar(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c1, 0" : [out] "=r" (val) :
	);
	return val;
}

uint32_t _get_plepcr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c1, 1" : [out] "=r" (val) :
	);
	return val;
}


/*******************************
 Identification registers
 *******************************/
/* Main ID Register */
uint32_t _get_midr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

/* Cache Type Register */
uint32_t _get_ctr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

/* TCM Type Register */
uint32_t _get_tcmtr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 2" : [out] "=r" (val) :
	);
	return val;
}

/* TLB Type Register */
uint32_t _get_tlbtr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 3" : [out] "=r" (val) :
	);
	return val;
}

/* Multiprocessor Affinity Register */
uint32_t _get_mpidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 5" : [out] "=r" (val) :
	);
	return val;
}

/* Revision ID register */
uint32_t _get_revidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 6" : [out] "=r" (val) :
	);
	return val;
}
/* Processor Feature Register 0 */
uint32_t _get_pfr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 0" : [out] "=r" (val) :
	);
	return val;
}
/* Processor Feature Register 1 */
uint32_t _get_pfr1(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 1" : [out] "=r" (val) :
	);
	return val;
}
/* Debug Feature Register 0 */
uint32_t _get_dfr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 2" : [out] "=r" (val) :
	);
	return val;
}
/* Auxiliary Feature Register 0 */
uint32_t _get_afr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 3" : [out] "=r" (val) :
	);
	return val;
}
/* Memory Model Feature Register 0 */
uint32_t _get_mmfr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 4" : [out] "=r" (val) :
	);
	return val;
}
/* Memory Model Feature Register 1 */
uint32_t _get_mmfr1(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 5" : [out] "=r" (val) :
	);
	return val;
}
/*Memory Model Feature Register 2 */
uint32_t _get_mmfr2(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 6" : [out] "=r" (val) :
	);
	return val;
}
/* Memory Model Feature Register 3 */
uint32_t _get_mmfr3(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 7" : [out] "=r" (val) :
	);
	return val;
}

/* Instruction Set Attribute Register 0 */
uint32_t _get_isar0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 0" : [out] "=r" (val) :
	);
	return val;
}
/* Instruction Set Attribute Register 1 */
uint32_t _get_isar1(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 1" : [out] "=r" (val) :
	);
	return val;
}
/* Instruction Set Attribute Register 2 */
uint32_t _get_isar2(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 2" : [out] "=r" (val) :
	);
	return val;
}
/* Instruction Set Attribute Register 0 */
uint32_t _get_isar3(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 3" : [out] "=r" (val) :
	);
	return val;
}
/* Instruction Set Attribute Register 4 */
uint32_t _get_isar4(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 4" : [out] "=r" (val) :
	);
	return val;
}
/* Cache Size Identification Register */
uint32_t _get_ccsidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 1, %[out], c0, c0, 0" : [out] "=r" (val) :
	);
	return val;
}
/* Cache Level ID Register */
uint32_t _get_clidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 1, %[out], c0, c0, 1" : [out] "=r" (val) :
	);
	return val;
}
/* Auxiliary ID Register */
uint32_t _get_aidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 1, %[out], c0, c0, 7" : [out] "=r" (val) :
	);
	return val;
}
/* Cache Size Selection Register */
uint32_t _get_csselr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 2, %[out], c0, c0, 0" : [out] "=r" (val) :
	);
	return val;
}
void _print_mmu_regs(void) {
	/* Sometimes accessing this registers crushes the emulator */
#if LOG_LEVEL > 0
	log_debug("ARM MMU registers summary:");
	log_debug("TLB Type:                  %#10x", _get_mmu_tlb_type());
	log_debug("SCTRL:                     %#10x", _get_sctrl_control());
	log_debug("ACTRL:                     %#10x", _get_actrl_control());
	log_debug("CPACR:                     %#10x", _get_cpacr_control());
	log_debug("Non-Secure Access Control: %#10x", _get_mmu_nonsecure_access_control());
	log_debug("Translation Table Base 0:  %#10x", _get_mmu_translation_table_base_0());
	log_debug("Translation Table Base 1:  %#10x", _get_mmu_translation_table_base_1());
	log_debug("Domain Access Conrol:      %#10x", _get_mmu_domain_access_control());
	log_debug("Data Fault Status:         %#10x", _get_mmu_data_fault_status());
	log_debug("Instruction Fault Status:  %#10x", _get_mmu_instruction_fault_status());
	log_debug("Data Fault Address:        %#10x", _get_mmu_data_fault_address());
	log_debug("Instruction Fault Address: %#10x", _get_mmu_instruction_fault_address());
	log_debug("TLB lockdown:              %#10x", _get_mmu_tlb_lockdown());
	log_debug("Primary Region Remap:      %#10x", _get_mmu_primary_region_remap());
	log_debug("Normal Memory Remap:       %#10x", _get_mmu_normal_memory_remap());
	log_debug("FSCE PID:                  %#10x", _get_mmu_fsce_pid());
	log_debug("Context ID:                %#10x", _get_mmu_context_id());
	log_debug("Peripheral port remap:     %#10x", _get_mmu_peripheral_port_memory_remap());
	log_debug("TLB Lockdown Index:        %#10x", _get_mmu_tlb_lockdown_index());
	log_debug("TLB Lockdown VA:           %#10x", _get_mmu_tlb_lockdown_va());
	log_debug("TLB Lockdown PA:           %#10x", _get_mmu_tlb_lockdown_pa());
	log_debug("TLB Lockdown Attribues:    %#10x", _get_mmu_tlb_lockdown_attributes());

	log_debug("PLEIDR:                    %#10x", _get_pleidr());
	
	if (_get_pleidr()) {
		log_debug("PLEASR:                    %#10x", _get_pleasr());
		log_debug("PLESFR:                    %#10x", _get_plesfr());
		log_debug("PLEAUR:                    %#10x", _get_pleuar());
		log_debug("PLEPCR:                    %#10x", _get_plepcr());
	}


	log_debug("MIDR:                      %#10x", _get_midr());
	log_debug("CTR:                       %#10x", _get_ctr());
	log_debug("TCMTR:                     %#10x", _get_tcmtr());
	log_debug("TLBTR:                     %#10x", _get_tlbtr());
	log_debug("MPIDR:                     %#10x", _get_mpidr());
	log_debug("REVIDR:                    %#10x", _get_revidr());

	log_debug("PFR0:                      %#10x", _get_pfr0());
	log_debug("PFR1:                      %#10x", _get_pfr1());
	log_debug("DFR0:                      %#10x", _get_dfr0());
	log_debug("AFR0:                      %#10x", _get_afr0());

	log_debug("MMFR0:                      %#10x", _get_mmfr0());
	log_debug("MMFR1:                      %#10x", _get_mmfr1());
	log_debug("MMFR2:                      %#10x", _get_mmfr2());
	log_debug("MMFR3:                      %#10x", _get_mmfr3());

	log_debug("ISAR0:                      %#10x", _get_isar0());
	log_debug("ISAR1:                      %#10x", _get_isar1());
	log_debug("ISAR2:                      %#10x", _get_isar2());
	log_debug("ISAR3:                      %#10x", _get_isar3());
	log_debug("ISAR4:                      %#10x", _get_isar4());

	log_debug("CCSIDR:                     %#10x", _get_ccsidr());
	log_debug("CLIDR:                      %#10x", _get_clidr());
	log_debug("AIDR:                       %#10x", _get_aidr());
	log_debug("CSSELR:                     %#10x", _get_csselr());
#endif
}
