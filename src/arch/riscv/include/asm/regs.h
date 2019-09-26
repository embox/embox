#ifndef RISCV_REGISTERS_H_
#define RISCV_REGISTERS_H_

/* Machine mode Status Register (mstatus) */
#define MSTATUS_MIE 	(1 << 3) 		/* Machine Interrupt Enable */
#define MSTATUS_MPIE 	(1 << 7)		/* Machine Previous Interrupt Enable */
#define MSTATUS_MPP 	(3 << 11)		/* Machine Previous Privilege Mode */

/* Machine Mode Interrupt Register (mie)*/
#define MIE_MSIE (1 << 3)	/* Machine Software Interrupt Enable */
#define MIE_MTIE (1 << 7)	/* Machine Timer Interrupt Enable */
#define MIE_MEIE (1 << 11)	/* Machine External Interrupt Enable */

/* Machine Interrupt Pending (mip) */
#define MIP_MSIP (1 << 3)	/* Machine Software Interrupt Pending */
#define MIP_MTIP (1 << 7) 	/* Machine Timer Interrupt Pending */
#define MIP_MEIP (1 << 11 	/* Machine External Interrupt Pending */

#define __ENABLE_TIMER_INTERRUPTS \
	__asm volatile("csrs mie,%0"::"r"(MIE_MTIE));

#define read_csr(reg) ({ unsigned long __tmp; \
  __asm volatile ("csrr %0, " #reg : "=r"(__tmp)); __tmp; })

#define write_csr(reg, val) ({ \
  __asm volatile ("csrw " #reg ", %0" :: "rK"((val))); })

#define set_csr_bit(reg, mask) ({ \
  __asm volatile ("csrs " #reg ", %0" :: "rK"((mask))); })

#define clear_csr_bit(reg, mask) ({ \
  __asm volatile ("csrc " #reg ", %0" :: "rK"((mask))); })

#endif /* RISCV_REGISTERS_H_ */
