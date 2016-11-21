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

#include <framework/mod/options.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(mmu_init);

#define DOMAIN_ACCESS OPTION_GET(NUMBER, domain_access)
#define CTX_NUMBER    32 /* TODO: make it related to number of tasks */

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

void mmu_regs(void);

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

	mmu_regs();
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
void mmu_regs(void) {
	printk("ARM MMU registers summary:\n");
	printk("TLB Type:                  %#10x\n", _get_mmu_tlb_type());
	printk("SCTRL:                     %#10x\n", _get_sctrl_control());
	printk("ACTRL:                     %#10x\n", _get_actrl_control());
	printk("CPACR:                     %#10x\n", _get_cpacr_control());
	printk("Non-Secure Access Control: %#10x\n", _get_mmu_nonsecure_access_control());
	printk("Translation Table Base 0:  %#10x\n", _get_mmu_translation_table_base_0());
	printk("Translation Table Base 1:  %#10x\n", _get_mmu_translation_table_base_1());
	printk("Domain Access Conrol:      %#10x\n", _get_mmu_domain_access_control());
	printk("Data Fault Status:         %#10x\n", _get_mmu_data_fault_status());
	printk("Instruction Fault Status:  %#10x\n", _get_mmu_instruction_fault_status());
	printk("Data Fault Address:        %#10x\n", _get_mmu_data_fault_address());
	printk("Instruction Fault Address: %#10x\n", _get_mmu_instruction_fault_address());
	printk("TLB lockdown:              %#10x\n", _get_mmu_tlb_lockdown());
	printk("Primary Region Remap:      %#10x\n", _get_mmu_primary_region_remap());
	printk("Normal Memory Remap:       %#10x\n", _get_mmu_normal_memory_remap());
	printk("FSCE PID:                  %#10x\n", _get_mmu_fsce_pid());
	printk("Context ID:                %#10x\n", _get_mmu_context_id());
	printk("Peripheral port remap:     %#10x\n", _get_mmu_peripheral_port_memory_remap());
	printk("TLB Lockdown Index:        %#10x\n", _get_mmu_tlb_lockdown_index());
	printk("TLB Lockdown VA:           %#10x\n", _get_mmu_tlb_lockdown_va());
	printk("TLB Lockdown PA:           %#10x\n", _get_mmu_tlb_lockdown_pa());
	printk("TLB Lockdown Attribues:    %#10x\n", _get_mmu_tlb_lockdown_attributes());

	printk("PLEIDR:                    %#10x\n", _get_pleidr());
	
	if (_get_pleidr()) {
		printk("PLEASR:                    %#10x\n", _get_pleasr());
		printk("PLESFR:                    %#10x\n", _get_plesfr());
		printk("PLEAUR:                    %#10x\n", _get_pleuar());
		printk("PLEPCR:                    %#10x\n", _get_plepcr());
	}


	printk("MIDR:                      %#10x\n", _get_midr());
	printk("CTR:                       %#10x\n", _get_ctr());
	printk("TCMTR:                     %#10x\n", _get_tcmtr());
	printk("TLBTR:                     %#10x\n", _get_tlbtr());
	printk("MPIDR:                     %#10x\n", _get_mpidr());
	printk("REVIDR:                    %#10x\n", _get_revidr());

	printk("PFR0:                      %#10x\n", _get_pfr0());
	printk("PFR1:                      %#10x\n", _get_pfr1());
	printk("DFR0:                      %#10x\n", _get_dfr0());
	printk("AFR0:                      %#10x\n", _get_afr0());

	printk("MMFR0:                      %#10x\n", _get_mmfr0());
	printk("MMFR1:                      %#10x\n", _get_mmfr1());
	printk("MMFR2:                      %#10x\n", _get_mmfr2());
	printk("MMFR3:                      %#10x\n", _get_mmfr3());

	printk("ISAR0:                      %#10x\n", _get_isar0());
	printk("ISAR1:                      %#10x\n", _get_isar1());
	printk("ISAR2:                      %#10x\n", _get_isar2());
	printk("ISAR3:                      %#10x\n", _get_isar3());
	printk("ISAR4:                      %#10x\n", _get_isar4());

	printk("CCSIDR:                     %#10x\n", _get_ccsidr());
	printk("CLIDR:                      %#10x\n", _get_clidr());
	printk("AIDR:                       %#10x\n", _get_aidr());
	printk("CSSELR:                     %#10x\n", _get_csselr());

}


