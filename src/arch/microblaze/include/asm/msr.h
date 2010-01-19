/*
 * msr.h
 *
 *  Created on: 11.01.2010
 *      Author: fomka
 */

#ifndef MSR_H_
#define MSR_H_


#ifdef REVERSE_MASK
#undef REVERSE_MASK
#endif
/* It's necessary to put 31 here
 * because microblaze has reverse bits order */
#define REVERSE_MASK(bit_num) (1<<(31-bit_num))

#define MSR_EIP_BIT    22
#define MSR_EE_BIT     23
#define MSR_DCE_BIT    24
#define MSR_DZ_BIT     25
#define MSR_ICE_BIT    26
#define MSR_FSL_BIT    27
#define MSR_BIP_BIT    28
#define MSR_IE_BIT     29
#define MSR_BE_BIT     30
#define MSR_C_BIT      31

#define MSR_EIP_MASK	REVERSE_MASK(MSR_EIP_BIT)
#define MSR_EE_MASK	REVERSE_MASK(MSR_EE_BIT)
#define MSR_DCE_MASK	REVERSE_MASK(MSR_DCE_BIT)
#define MSR_DZ_MASK    REVERSE_MASK(MSR_DZ_BIT)
#define MSR_ICE_MASK   REVERSE_MASK(MSR_ICE_BIT)
#define MSR_FSL_MASK   REVERSE_MASK(MSR_FSL_BIT)
#define MSR_BIP_MASK   REVERSE_MASK(MSR_BIP_BIT)
#define MSR_IE_MASK    REVERSE_MASK(MSR_IE_BIT)
#define MSR_BE_MASK    REVERSE_MASK(MSR_BE_BIT)
#define MSR_C_MASK     REVERSE_MASK(MSR_C_BIT)

/* code from u-boot (modified)*/
/* use machine status register USE_MSR_REG */
#ifdef XILINX_USE_MSR_INSTR
#define msr_set(val) \
	__asm__ __volatile__ ("msrset r0, %0" ::"i"(val) );

#define msr_clr(val) \
	__asm__ __volatile__ ("msrclr r0, %0" ::"i"(val));

#else
#define msr_set(val)                		\
{                                         	\
	register unsigned tmp;             	\
	__asm__ __volatile__ ("          		\
			mfs	%0, rmsr;                   \
			ori	%0, %0, %1;             	\
			mts	rmsr, %0;                   \
			nop;"                           \
			: "=r" (tmp)                    \
			: "i" (val)                     \
			: "memory");                    \
}

#define msr_clr(val)						\
{											\
	register unsigned tmp;				\
	__asm__ __volatile__ ("				\
			mfs	%0, rmsr;					\
			andi	%0, %0, ~%1;			\
			mts	rmsr, %0;					\
			nop;"							\
			: "=r" (tmp)					\
			: "i" (val)						\
			: "memory");					\
}
#endif

#define msr_set_bit(bit) \
	msr_set(REVERSE_MASK(bit))

#define msr_clr_bit(bit) \
	msr_clr(REVERSE_MASK(bit))

#endif /* MSR_H_ */
