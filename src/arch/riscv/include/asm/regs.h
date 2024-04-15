#ifndef RISCV_REGISTERS_H_
#define RISCV_REGISTERS_H_

#include <framework/mod/options.h>

#include "config/embox/arch/riscv/kernel/conf.h"
#include "util/macro.h"

#define SMODE \
	OPTION_MODULE_GET(embox__arch__riscv__kernel__conf, BOOLEAN, smode)

#if SMODE
#define MODE_LOWER_CASE s
#define MODE_UPPER_CASE S
#else
#define MODE_LOWER_CASE m
#define MODE_UPPER_CASE M
#endif
#define RET_INSTR       MACRO_CONCAT(MODE_LOWER_CASE, ret)
#define CAUSE_REG       MACRO_CONCAT(MODE_LOWER_CASE, cause)
#define STATUS_REG      MACRO_CONCAT(MODE_LOWER_CASE, status)
#define INTERRUPT_REG   MACRO_CONCAT(MODE_LOWER_CASE, ie)
#define EPC_REG         MACRO_CONCAT(MODE_LOWER_CASE, epc)
#define TRAP_VECTOR_REG MACRO_CONCAT(MODE_LOWER_CASE, tvec)

/* Machine mode Status Register (mstatus) */
#define MSTATUS_MIE  (1UL << 3)  /* Machine Interrupt Enable */
#define MSTATUS_MPIE (1UL << 7)  /* Machine Previous Interrupt Enable */
#define MSTATUS_MPP  (3UL << 11) /* Machine Previous Privilege Mode */

#define SSTATUS_SIE  (1UL << 1) /* Supervisor Interrupt Enable */
#define SSTATUS_SPIE (1UL << 5) /* Supervisor Previous Interrupt Enable */
#define SSTATUS_SPP  (1UL << 8) /* Supervisor Previous Privilege Mode */

#define STATUS(x)                                        \
	MACRO_CONCAT(MACRO_CONCAT(MODE_UPPER_CASE, STATUS_), \
	    MACRO_CONCAT(MODE_UPPER_CASE, x))

/* Machine Mode Interrupt Register (mie)*/
#define MIE_MSIE (1UL << 3)  /* Machine Software Interrupt Enable */
#define MIE_MTIE (1UL << 7)  /* Machine Timer Interrupt Enable */
#define MIE_MEIE (1UL << 11) /* Machine External Interrupt Enable */

/* Supervisor Mode Interrupt Register (sie)*/
#define SIE_SSIE (1UL << 1) /* Supervisor Software Interrupt Enable */
#define SIE_STIE (1UL << 5) /* Supervisor Timer Interrupt Enable */
#define SIE_SEIE (1UL << 9) /* Supervisor External Interrupt Enable */

#define IE(x)                                        \
	MACRO_CONCAT(MACRO_CONCAT(MODE_UPPER_CASE, IE_), \
	    MACRO_CONCAT(MODE_UPPER_CASE, x))

/* Machine Interrupt Pending (mip) */
#define MIP_MSIP (1UL << 3)  /* Machine Software Interrupt Pending */
#define MIP_MTIP (1UL << 7)  /* Machine Timer Interrupt Pending */
#define MIP_MEIP (1UL << 11) /* Machine External Interrupt Pending */

/* Match Control (mcontrol) */
#define MCONTROL_DMODE   (1UL << (__riscv_xlen - 5))
#define MCONTROL_SELECT  (1UL << 19)
#define MCONTROL_TIMING  (1UL << 18)
#define MCONTROL_SIZELO  (3UL << 16)
#define MCONTROL_ACTION  (0xfUL << 12)
#define MCONTROL_CHAIN   (1UL << 11)
#define MCONTROL_MATCH   (0xfUL << 7)
#define MCONTROL_M       (1UL << 6)
#define MCONTROL_H       (1UL << 5)
#define MCONTROL_S       (1UL << 4)
#define MCONTROL_U       (1UL << 3)
#define MCONTROL_EXECUTE (1UL << 2)
#define MCONTROL_STORE   (1UL << 1)
#define MCONTROL_LOAD    (1UL << 0)

#define MCONTROL_ACTION_DEBUG_EXCEPTION (0UL << 12)
#define MCONTROL_ACTION_DEBUG_MODE      (1UL << 12)
#define MCONTROL_ACTION_TRACE_START     (2UL << 12)
#define MCONTROL_ACTION_TRACE_STOP      (3UL << 12)
#define MCONTROL_ACTION_TRACE_EMIT      (4UL << 12)

#define MCONTROL_MATCH_EQUAL     (0UL << 7)
#define MCONTROL_MATCH_NAPOT     (1UL << 7)
#define MCONTROL_MATCH_GE        (2UL << 7)
#define MCONTROL_MATCH_LT        (3UL << 7)
#define MCONTROL_MATCH_MASK_LOW  (4UL << 7)
#define MCONTROL_MATCH_MASK_HIGH (5UL << 7)

/* Machine Cause (mcause) */
/* Asynchronous causes */
#define IRQ_MACHINE_SOFTWARE 3
#define IRQ_MACHINE_TIMER    7
#define IRQ_MACHINE_EXTERNAL 11
/* Synchronous causes */
#define EXC_INSTR_ALIGN  0
#define EXC_INSTR_ACCESS 1
#define EXC_INSTR_LEGAL  2
#define EXC_BREAKPOINT   3
#define EXC_LOAD_ALIGN   4
#define EXC_LOAD_ACCESS  5
#define EXC_STORE_ALIGN  6
#define EXC_STORE_ACCESS 7

/* Supervisor Cause (scause) */
/* Asynchronous causes */
#define IRQ_SUPERVISOR_SOFTWARE 1
#define IRQ_SUPERVISOR_TIMER    5
#define IRQ_SUPERVISOR_EXTERNAL 9

#if SMODE
#define IRQ_SOFTWARE IRQ_SUPERVISOR_SOFTWARE
#define IRQ_TIMER    IRQ_SUPERVISOR_TIMER
#define IRQ_EXTERNAL IRQ_SUPERVISOR_EXTERNAL
#else
#define IRQ_SOFTWARE IRQ_MACHINE_SOFTWARE
#define IRQ_TIMER    IRQ_MACHINE_TIMER
#define IRQ_EXTERNAL IRQ_MACHINE_EXTERNAL
#endif

#define __ENABLE_TIMER_INTERRUPTS __asm volatile("csrs mie,%0" ::"r"(MIE_MTIE));

#define read_csr(reg)                                                \
	({                                                               \
		unsigned long __tmp;                                         \
		__asm volatile("csrr %0, " MACRO_STRING(reg) : "=r"(__tmp)); \
		__tmp;                                                       \
	})

#define write_csr(reg, val) \
	({ __asm volatile("csrw " MACRO_STRING(reg) ", %0" ::"rK"((val))); })

#define set_csr_bit(reg, mask) \
	({ __asm volatile("csrs " MACRO_STRING(reg) ", %0" ::"rK"((mask))); })

#define clear_csr_bit(reg, mask) \
	({ __asm volatile("csrc " MACRO_STRING(reg) ", %0" ::"rK"((mask))); })

#endif /* RISCV_REGISTERS_H_ */
