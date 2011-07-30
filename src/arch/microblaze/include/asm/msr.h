/**
 * @file
 * @brief Includes framework for working with msr register in microblaze cpu
 *
 * @date 11.01.10
 * @author Alexey Fomin
 */

#ifndef MICROBLAZE_MSR_H_
#define MICROBLAZE_MSR_H_

#include <types.h>
#include <asm/bitops.h>

#define MSR_VMS_BIT    17
#define MSR_VM_BIT     18
#define MSR_UMS_BIT    19
#define MSR_UM_BIT     20
#define MSR_PVR_BIT    21
#define MSR_EIP_BIT    22
#define MSR_EE_BIT     23
#define MSR_DCE_BIT    24
#define MSR_DZ_BIT     25
#define MSR_ICE_BIT    26
#define MSR_FSL_BIT    27
#define MSR_BIP_BIT    28
#define MSR_C_BIT      29
#define MSR_IE_BIT     30
#define MSR_BE_BIT     31

#define MSR_VMS_MASK   REVERSE_MASK(MSR_VMS_BIT)
#define MSR_VM_MASK    REVERSE_MASK(MSR_VM_BIT)
#define MSR_UMS_MASK   REVERSE_MASK(MSR_UMS_BIT)
#define MSR_UM_MASK    REVERSE_MASK(MSR_UM_BIT)
#define MSR_PVR_MASK   REVERSE_MASK(MSR_PVR_BIT)
#define MSR_EIP_MASK   REVERSE_MASK(MSR_EIP_BIT)
#define MSR_EE_MASK    REVERSE_MASK(MSR_EE_BIT)
#define MSR_DCE_MASK   REVERSE_MASK(MSR_DCE_BIT)
#define MSR_DZ_MASK    REVERSE_MASK(MSR_DZ_BIT)
#define MSR_ICE_MASK   REVERSE_MASK(MSR_ICE_BIT)
#define MSR_FSL_MASK   REVERSE_MASK(MSR_FSL_BIT)
#define MSR_BIP_MASK   REVERSE_MASK(MSR_BIP_BIT)
#define MSR_IE_MASK    REVERSE_MASK(MSR_IE_BIT)
#define MSR_BE_MASK    REVERSE_MASK(MSR_BE_BIT)
#define MSR_C_MASK     REVERSE_MASK(MSR_C_BIT)

#ifndef __ASSEMBLER__

#define XILINX_USE_MSR_INSTR
/* code from u-boot (modified)*/
/* use machine status register USE_MSR_REG */
#ifdef XILINX_USE_MSR_INSTR
/**
 * Sets some bits in msr register
 * @val bit's mask want to be set
 */
static inline void msr_set(uint32_t val) {
	__asm__ __volatile__ ("msrset r0, %0" ::"i"(val));
}
/**
 * Clears some bits in msr register
 * @val bit's mask want to be cleared
 */
static inline void msr_clr(uint32_t val) {
	__asm__ __volatile__ ("msrclr r0, %0" ::"i"(val));
}

#else

/**
 * Sets some bits in msr register
 * @val bit's mask want to be set
 */
static inline void msr_set(uint32_t val) {
	register unsigned tmp;
	__asm__ __volatile__ (
		"mfs	%0, rmsr;\n\t"
		"ori	%0, %0, %1;\n\t"
		"mts	rmsr, %0;\n\t"
		"nop;"
		: "=r" (tmp)
		: "r" (val)
		: "memory"
	);
}

/**
 * Clears some bits in msr register
 * @val bit's mask want to be cleared
 */
static inline void msr_clr(uint32_t val) {
	register unsigned tmp;
	__asm__ __volatile__ (
		"mfs	%0, rmsr;\n\t"
		"andni	%0, %0, %1;\n\t"
		"mts	rmsr, %0;\n\t"
		"nop;"
		: "=r" (tmp)
		: "r" (val)
		: "memory"
	);
}
#endif /* XILINX_USE_MSR_INSTR */

/**
 * Clears some bits in msr register
 * @val bit's mask want to be cleared
 */
static inline uint32_t msr_get_value(void) {
	register unsigned msr;
	__asm__ __volatile__ (
		"mfs	%0, rmsr;\n\t"
		: "=r" (msr)
	);
	return msr;
}

static inline void msr_set_value(uint32_t val) {
	__asm__ __volatile__ (
		"mts	rmsr, %0;\n\t"
		:
		: "r" (val)
	);
}

static inline void msr_set_ie(void) {
	__asm__ __volatile__ ("msrset r0, %0" ::"i"(REVERSE_MASK(MSR_IE_BIT)));
}

static inline void msr_clr_ie(void) {
	__asm__ __volatile__ ("msrclr r0, %0" ::"i"(REVERSE_MASK(MSR_IE_BIT)));
}

static inline void msr_set_ee(void) {
	__asm__ __volatile__ ("msrset r0, %0" ::"i"(REVERSE_MASK(MSR_EE_BIT)));
}

static inline void msr_clr_ee(void) {
	__asm__ __volatile__ ("msrclr r0, %0" ::"i"(REVERSE_MASK(MSR_EE_BIT)));
}

static inline uint32_t msr_get_bit(int bit) {
	return msr_get_value() & REVERSE_MASK(bit);
}

#endif /* __ASSEMBLER__ */

#endif /* MICROBLAZE_MSR_H_ */
