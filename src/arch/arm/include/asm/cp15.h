/**
 * @file
 *
 * @date 05.04.2017
 * @author Anton Bondarev
 */

#ifndef CP15_H_
#define CP15_H_

#include <stdint.h>

#include <hal/mem_barriers.h>

#ifndef __ASSEMBLER__

static inline uint32_t cp15_get_sctrl(void) {
	uint32_t val;
	asm("mrc p15, 0, %0, c1, c0, 0    @ get SCTRL" : "=r" (val) : : "cc");
	return val;
}

static inline void cp15_set_sctrl(uint32_t val)
{
	asm volatile("mcr p15, 0, %0, c1, c0, 0    @ set SCTRL"
	  : : "r" (val) : "cc");
	isb();
}

static inline uint32_t cp15_get_actrl(void) {
	uint32_t val;
	asm("mrc p15, 0, %0, c1, c0, 1    @ get ACTLR" : "=r" (val) : : "cc");
	return val;
}

static inline void cp15_set_actrl(uint32_t val)
{
	asm volatile("mcr p15, 0, %0, c1, c0, 1    @ set ACTLR"
	  : : "r" (val));
	isb();
}

/* Coprocessor Access Control Register */
static inline uint32_t cp15_get_cpacr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %0, c1, c0, 2    @ get CPACR" : "=r" (val) : : "cc");
	return val;
}

static inline void cp15_set_cpacr(uint32_t val) {
	asm volatile("mcr p15, 0, %0, c1, c0, 2    @ set CPACR"
	  : : "r" (val) : "cc");
	isb();
}

static inline uint32_t cp15_get_scr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %0, c1, c1, 0    @ get SCR" : "=r" (val) : : "cc");
	return val;
}

static inline void cp15_set_scr(uint32_t val) {
	asm volatile("mcr p15, 0, %0, c1, c1, 0    @ set SCR"
	  : : "r" (val) : "cc");
	isb();
}

static inline uint32_t cp15_get_sder(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %0, c1, c1, 1    @ get SDER" : "=r" (val) : : "cc");
	return val;
}

static inline void cp15_set_sder(uint32_t val) {
	asm volatile("mcr p15, 0, %0, c1, c1, 1    @ set SDER"
	  : : "r" (val) : "cc");
	isb();
}

static inline uint32_t cp15_get_nsacr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %0, c1, c1, 2    @ get NSACR" : "=r" (val) : : "cc");
	return val;
}

static inline void cp15_set_nsacr(uint32_t val) {
	asm volatile("mcr p15, 0, %0, c1, c1, 2    @ set NSACR"
	  : : "r" (val) : "cc");
	isb();
}

static inline uint32_t cp15_get_mvbar(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %0, c12, c0, 1    @ get MVBAR" : "=r" (val) : : "cc");
	return val;
}

static inline uint32_t cp15_get_vbar(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %0, c12, c0, 0    @ get VBAR" : "=r" (val) : : "cc");
	return val;
}

/* Software accessible MMU registers */
static inline uint32_t cp15_get_mmu_tlb_type(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 3" : [out] "=r" (val) :
	);
	return val;
}


static inline uint32_t cp15_get_translation_table_base_0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c2, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_translation_table_base_1(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c2, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_domain_access_control(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c3, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_data_fault_status(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c5, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_instruction_fault_status(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c5, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_data_fault_address(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c6, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_instruction_fault_address(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c6, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_tlb_lockdown(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c10, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_primary_region_remap(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c10, c2, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline uint32_t cp15_get_normal_memory_remap(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c10, c2, 1" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_fsce_pid(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c13, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_contextidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c13, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

static inline void arm_set_contextidr(uint32_t val) {
	__asm__ __volatile__ (
		"mcr p15, 0, %0, c13, c0, 1\n\t"
		: : "r" (val) :
	);
}

#define TTBR0_ADDR_MASK 0xFFFFFF00
static inline uint32_t cp15_get_ttbr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c2, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

#define TTBCR_PAE_SUPPORT 0x80000000
static inline uint32_t cp15_get_ttbcr(void) {
	uint32_t val;
	__asm__ __volatile__("mrc p15, 0, %[out], c2, c0, 2" : [out] "=r"(val) :);
	return val;
}

static inline void cp15_set_ttbcr(uint32_t val) {
	__asm__ __volatile__("mcr p15, 0, %0, c2, c0, 2\n\t" : : "r"(val) :);
}

static inline void cp15_set_ttbr0(uint32_t val) {
	__asm__ __volatile__("mcr p15, 0, %0, c2, c0, 0" : : "r"(val));
}

static inline void cp15_set_ttbr1(uint32_t val) {
	__asm__ __volatile__ (
		"mcr p15, 0, %0, c2, c0, 1" : : "r"(val)
	);
}

#ifdef CORTEX_A9
/* CP15 c15 implemented */
static inline int32_t cp15_get_mmu_peripheral_port_memory_remap(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c2, 4" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_mmu_tlb_lockdown_index(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c4, 2" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_mmu_tlb_lockdown_va(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c5, 2" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_mmu_tlb_lockdown_pa(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c6, 2" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_mmu_tlb_lockdown_attributes(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c15, c7, 2" : [out] "=r" (val) :
	);
	return val;
}
#endif

#ifdef CORTEX_A9
/* CP15 c11, Reserved for TCM DMA registers */
static inline int32_t cp15_get_pleidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_pleasr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c0, 2" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_plesfr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c0, 4" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_pleuar(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c1, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline int32_t cp15_get_plepcr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c11, c1, 1" : [out] "=r" (val) :
	);
	return val;
}
#endif

/*******************************
 Identification registers
 *******************************/
/* Main ID Register */
static inline int32_t cp15_get_midr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

/* Cache Type Register */
static inline int32_t cp15_get_ctr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 1" : [out] "=r" (val) :
	);
	return val;
}

/* TCM Type Register */
static inline int32_t cp15_get_tcmtr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 2" : [out] "=r" (val) :
	);
	return val;
}

/* TLB Type Register */
static inline int32_t cp15_get_tlbtr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 3" : [out] "=r" (val) :
	);
	return val;
}

/* Multiprocessor Affinity Register */
static inline int32_t cp15_get_mpidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 5" : [out] "=r" (val) :
	);
	return val;
}

/* Revision ID register */
static inline int32_t cp15_get_revidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c0, 6" : [out] "=r" (val) :
	);
	return val;
}
/* Processor Feature Register 0 */
static inline int32_t cp15_get_pfr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 0" : [out] "=r" (val) :
	);
	return val;
}
/* Processor Feature Register 1 */
static inline int32_t cp15_get_pfr1(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 1" : [out] "=r" (val) :
	);
	return val;
}
/* Debug Feature Register 0 */
static inline int32_t cp15_get_dfr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 2" : [out] "=r" (val) :
	);
	return val;
}
/* Auxiliary Feature Register 0 */
static inline int32_t cp15_get_afr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 3" : [out] "=r" (val) :
	);
	return val;
}
/* Memory Model Feature Register 0 */
static inline int32_t cp15_get_mmfr0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 4" : [out] "=r" (val) :
	);
	return val;
}
/* Memory Model Feature Register 1 */
static inline int32_t cp15_get_mmfr1(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 5" : [out] "=r" (val) :
	);
	return val;
}
/*Memory Model Feature Register 2 */
static inline int32_t cp15_get_mmfr2(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 6" : [out] "=r" (val) :
	);
	return val;
}
/* Memory Model Feature Register 3 */
static inline int32_t cp15_get_mmfr3(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c1, 7" : [out] "=r" (val) :
	);
	return val;
}

/* Instruction Set Attribute Register 0 */
static inline int32_t cp15_get_isar0(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 0" : [out] "=r" (val) :
	);
	return val;
}
/* Instruction Set Attribute Register 1 */
static inline int32_t cp15_get_isar1(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 1" : [out] "=r" (val) :
	);
	return val;
}
/* Instruction Set Attribute Register 2 */
static inline int32_t cp15_get_isar2(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 2" : [out] "=r" (val) :
	);
	return val;
}
/* Instruction Set Attribute Register 0 */
static inline int32_t cp15_get_isar3(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 3" : [out] "=r" (val) :
	);
	return val;
}
/* Instruction Set Attribute Register 4 */
static inline int32_t cp15_get_isar4(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c0, c2, 4" : [out] "=r" (val) :
	);
	return val;
}
/* Cache Size Identification Register */
static inline int32_t cp15_get_ccsidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 1, %[out], c0, c0, 0" : [out] "=r" (val) :
	);
	return val;
}
/* Cache Level ID Register */
static inline int32_t cp15_get_clidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 1, %[out], c0, c0, 1" : [out] "=r" (val) :
	);
	return val;
}
/* Auxiliary ID Register */
static inline int32_t cp15_get_aidr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 1, %[out], c0, c0, 7" : [out] "=r" (val) :
	);
	return val;
}
/* Cache Size Selection Register */
static inline int32_t cp15_get_csselr(void) {
	uint32_t val;
	__asm__ __volatile__ (
		"mrc p15, 2, %[out], c0, c0, 0" : [out] "=r" (val) :
	);
	return val;
}

static inline void cp15_icache_inval(void) {
	__asm__ __volatile__("mcr p15, 0, %0, c7, c5, 0" : : "r"(0) : "memory");
	isb();
}

#endif

/**
 * c1, Control Register
 * read: MRC p15, 0, <Rd>, c1, c0, 0
 * write: MCR p15, 0, <Rd>, c1, c0, 0
 */
#define CR_M   0x00000001 /* MMU enabled */
#define CR_A   0x00000002 /* strict alignment fault checking enabled */
#define CR_C   0x00000004 /* data caching enabled */
#define CR_W   0x00000008 /* Write buffer enable */
#define CR_P   0x00000010 /* 32-bit exception handler */
#define CR_D   0x00000020 /* 32-bit data address range */
#define CR_L   0x00000040 /* Implementation defined */
#define CR_B   0x00000080 /* Big endian */
#define CR_S   0x00000100 /* System MMU protection */
#define CR_R   0x00000200 /* ROM MMU protection */
#define CR_F   0x00000400 /* Implementation defined */
#define CR_Z   0x00000800 /* program flow prediction enabled */
#define CR_I   0x00001000 /* instruction caching enabled */
#define CR_V   0x00002000 /* Vectors relocated to 0xffff0000 */
#define CR_RR  0x00004000 /* Round Robin cache replacement */
#define CR_L4  0x00008000 /* LDR pc can set T bit */
#define CR_DT  0x00010000 /* (1 << 16) */
#if 0
#ifdef CONFIG_MMU
#define CR_HA  (1 << 17)  /* Hardware management of Access Flag   */
#else
#define CR_BR  (1 << 17)  /* MPU Background region enable (PMSA)  */
#endif
#endif
#define CR_HA  (1 << 17) /* Hardware management of Access Flag   */
#define CR_IT  (1 << 18)
#define CR_ST  (1 << 19)
#define CR_FI  (1 << 21) /* Fast interrupt (lower latency mode)	*/
#define CR_U   (1 << 22) /* Unaligned access operation		*/
#define CR_XP  (1 << 23) /* Extended page tables			*/
#define CR_VE  (1 << 24) /* Vectored interrupts			*/
#define CR_EE  (1 << 25) /* Exception (Big) Endian		*/
#define CR_TRE (1 << 28) /* TEX remap enable			*/
#define CR_AFE (1 << 29) /* Access flag enable			*/
#define CR_TE  (1 << 30) /* Thumb exception enable		*/


#endif /* CP15_H_ */
