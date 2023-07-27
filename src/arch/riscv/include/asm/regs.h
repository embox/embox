#ifndef RISCV_REGISTERS_H_
#define RISCV_REGISTERS_H_

/* Machine mode Status Register (mstatus) */
#define MSTATUS_MIE  (1UL << 3)  /* Machine Interrupt Enable */
#define MSTATUS_MPIE (1UL << 7)  /* Machine Previous Interrupt Enable */
#define MSTATUS_MPP  (3UL << 11) /* Machine Previous Privilege Mode */

/* Machine Mode Interrupt Register (mie)*/
#define MIE_MSIE (1UL << 3)  /* Machine Software Interrupt Enable */
#define MIE_MTIE (1UL << 7)  /* Machine Timer Interrupt Enable */
#define MIE_MEIE (1UL << 11) /* Machine External Interrupt Enable */

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

#define __ENABLE_TIMER_INTERRUPTS __asm volatile("csrs mie,%0" ::"r"(MIE_MTIE));

#define read_csr(reg)                                   \
	({                                                  \
		unsigned long __tmp;                            \
		__asm volatile("csrr %0, " #reg : "=r"(__tmp)); \
		__tmp;                                          \
	})

#define write_csr(reg, val) \
	({ __asm volatile("csrw " #reg ", %0" ::"rK"((val))); })

#define set_csr_bit(reg, mask) \
	({ __asm volatile("csrs " #reg ", %0" ::"rK"((mask))); })

#define clear_csr_bit(reg, mask) \
	({ __asm volatile("csrc " #reg ", %0" ::"rK"((mask))); })

#endif /* RISCV_REGISTERS_H_ */
