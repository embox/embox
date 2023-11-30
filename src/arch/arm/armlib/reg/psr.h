/**
 * @file
 * @brief
 *
 * @date 27.11.23
 * @author Aleksey Zhmulin
 */
#ifndef ARMLIB_REG_PSR_H_
#define ARMLIB_REG_PSR_H_

#include <stdint.h>

#define __MRS(reg)                                          \
	({                                                      \
		register uint32_t __val;                            \
		__asm__ __volatile__("mrs %0," #reg : "=r"(__val)); \
		__val;                                              \
	})

#define __MSR(reg, val)                                                \
	({                                                                 \
		register uint32_t __val = val;                                 \
		__asm__ __volatile__("msr " #reg ",%0" : : "r"(__val) : "cc"); \
	})

#ifndef __ASSEMBLER__
#define CPSR                        /* Current Program Status Register */
#define __ARCH_REG_LOAD__CPSR()     __MRS(cpsr)
#define __ARCH_REG_STORE__CPSR(val) __MSR(cpsr, val)

#define SPSR                        /* Current Program Status Register */
#define __ARCH_REG_LOAD__SPSR()     __MRS(spsr)
#define __ARCH_REG_STORE__SPSR(val) __MSR(spsr, val)
#endif /* __ASSEMBLER__ */

#define PSR_M       /* Current mode of the processor */
#define PSR_M_MASK  0b11111U
#define PSR_M_SHIFT 0
#define PSR_M_USR   0b10000U
#define PSR_M_FIQ   0b10001U
#define PSR_M_IRQ   0b10010U
#define PSR_M_SVC   0b10011U
#define PSR_M_MON   0b10110U
#define PSR_M_ABT   0b10111U
#define PSR_M_UND   0b11011U
#define PSR_M_SYS   0b11111U
#define PSR_T       (1U << 5) /* Thumb execution state bit */
#define PSR_F       (1U << 6) /* FIQ mask bit */
#define PSR_I       (1U << 7) /* IRQ mask bit */
#define PSR_A       (1U << 8) /* Asynchronous abort mask bit */
#define PSR_E       (1U << 9) /* Endianness execution state bit */

#endif /* ARMLIB_REG_PSR_H_ */
