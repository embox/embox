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
