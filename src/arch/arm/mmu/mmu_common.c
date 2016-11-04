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

uint32_t _get_mmu_control(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c1, c0, 0" : [out] "=r" (val) :
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

void mmu_regs(void) {
	printk("ARM MMU registers summary:\n");
	printk("TLB Type:                  %#10x\n", _get_mmu_tlb_type());
	printk("Control:                   %#10x\n", _get_mmu_control());
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
}
