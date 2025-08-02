/**
 * @file
 * @brief
 *
 * @date  14.05.2019
 * @author Dmitry Kurbatov
 */

#ifndef RISCV_ASM_CSR_H_
#define RISCV_ASM_CSR_H_

#include <framework/mod/options.h>
#include <riscv/smode.h>
#include <util/macro.h>

#if RISCV_SMODE
#define CSR_CAUSE  scause
#define CSR_STATUS sstatus
#define CSR_IE     sie
#define CSR_IP     sip
#define CSR_EPC    sepc
#define CSR_TVEC   stvec
#else
#define CSR_CAUSE  mcause
#define CSR_STATUS mstatus
#define CSR_IE     mie
#define CSR_IP     mip
#define CSR_EPC    mepc
#define CSR_TVEC   mtvec
#endif

/* Exception causes */
#define RISCV_EXC_INST_ALIGN   0
#define RISCV_EXC_INST_ACCESS  1
#define RISCV_EXC_INST_LEGAL   2
#define RISCV_EXC_BREAKPOINT   3
#define RISCV_EXC_LOAD_ALIGN   4
#define RISCV_EXC_LOAD_ACCESS  5
#define RISCV_EXC_STORE_ALIGN  6
#define RISCV_EXC_STORE_ACCESS 7

/* Interrupt causes */
#if RISCV_SMODE
#define RISCV_IRQ_SOFT  1
#define RISCV_IRQ_TIMER 5
#define RISCV_IRQ_EXT   9
#else
#define RISCV_IRQ_SOFT  3
#define RISCV_IRQ_TIMER 7
#define RISCV_IRQ_EXT   11
#endif

#define CSR_STATUS_SIE  (1UL << 1)  /* Supervisor Interrupt Enable */
#define CSR_STATUS_MIE  (1UL << 3)  /* Machine Interrupt Enable */
#define CSR_STATUS_SPIE (1UL << 5)  /* Supervisor Previous Interrupt Enable */
#define CSR_STATUS_MPIE (1UL << 7)  /* Machine Previous Interrupt Enable */
#define CSR_STATUS_SPP  (1UL << 8)  /* Supervisor Previous Privilege Mode */
#define CSR_STATUS_MPP  (3UL << 11) /* Machine Previous Privilege Mode */
#define CSR_STATUS_SD   (1UL << (__riscv_xlen - 1)) /* State Dirty */

#if RISCV_SMODE
#define CSR_STATUS_IE  CSR_STATUS_SIE
#define CSR_STATUS_PIE CSR_STATUS_SPIE
#define CSR_STATUS_PP  CSR_STATUS_SPP
#else
#define CSR_STATUS_IE  CSR_STATUS_MIE
#define CSR_STATUS_PIE CSR_STATUS_MPIE
#define CSR_STATUS_PP  CSR_STATUS_MPP
#endif

/* Floating point State */
#define CSR_STATUS_FS_MASK  3UL
#define CSR_STATUS_FS_SHIFT 13
#define CSR_STATUS_FS_OFF   0
#define CSR_STATUS_FS_INIT  1
#define CSR_STATUS_FS_CLEAN 2
#define CSR_STATUS_FS_DIRTY 3

#define CSR_IE_SIE (1UL << RISCV_IRQ_SOFT)  /* Software Interrupt Enable */
#define CSR_IE_TIE (1UL << RISCV_IRQ_TIMER) /* Timer Interrupt Enable */
#define CSR_IE_EIE (1UL << RISCV_IRQ_EXT)   /* External Interrupt Enable */

#define CSR_IP_SIP (1UL << RISCV_IRQ_SOFT)  /* Software Interrupt Pending */
#define CSR_IP_TIE (1UL << RISCV_IRQ_TIMER) /* Timer Interrupt Pending */
#define CSR_IP_EIE (1UL << RISCV_IRQ_EXT)   /* External Interrupt Pending */

#define csr_swap(csr, val)                                         \
	({                                                             \
		unsigned long __v = (unsigned long)(val);                  \
		__asm__ __volatile__("csrrw %0, " MACRO_STRING(csr) ", %1" \
		                     : "=r"(__v)                           \
		                     : "rK"(__v)                           \
		                     : "memory");                          \
		__v;                                                       \
	})

#define csr_read(csr)                                      \
	({                                                     \
		register unsigned long __v;                        \
		__asm__ __volatile__("csrr %0, " MACRO_STRING(csr) \
		                     : "=r"(__v)                   \
		                     :                             \
		                     : "memory");                  \
		__v;                                               \
	})

#define csr_write(csr, val)                                   \
	({                                                        \
		unsigned long __v = (unsigned long)(val);             \
		__asm__ __volatile__("csrw " MACRO_STRING(csr) ", %0" \
		                     :                                \
		                     : "rK"(__v)                      \
		                     : "memory");                     \
	})

#define csr_read_set(csr, val)                                     \
	({                                                             \
		unsigned long __v = (unsigned long)(val);                  \
		__asm__ __volatile__("csrrs %0, " MACRO_STRING(csr) ", %1" \
		                     : "=r"(__v)                           \
		                     : "rK"(__v)                           \
		                     : "memory");                          \
		__v;                                                       \
	})

#define csr_set(csr, val)                                     \
	({                                                        \
		unsigned long __v = (unsigned long)(val);             \
		__asm__ __volatile__("csrs " MACRO_STRING(csr) ", %0" \
		                     :                                \
		                     : "rK"(__v)                      \
		                     : "memory");                     \
	})

#define csr_read_clear(csr, val)                                   \
	({                                                             \
		unsigned long __v = (unsigned long)(val);                  \
		__asm__ __volatile__("csrrc %0, " MACRO_STRING(csr) ", %1" \
		                     : "=r"(__v)                           \
		                     : "rK"(__v)                           \
		                     : "memory");                          \
		__v;                                                       \
	})

#define csr_clear(csr, val)                                   \
	({                                                        \
		unsigned long __v = (unsigned long)(val);             \
		__asm__ __volatile__("csrc " MACRO_STRING(csr) ", %0" \
		                     :                                \
		                     : "rK"(__v)                      \
		                     : "memory");                     \
	})

#endif /* RISCV_ASM_CSR_H_ */
