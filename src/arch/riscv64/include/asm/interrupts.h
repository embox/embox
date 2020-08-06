#ifndef RISCV_INTERRUPTS_H_
#define RISCV_INTERRUPTS_H_

#include <asm/regs.h>

#define disable_interrupts()	clear_csr_bit(mstatus, MSTATUS_MIE)
#define enable_interrupts()	set_csr_bit(mstatus, MSTATUS_MIE)
// #define disable_interrupts()	__asm volatile  ( "csrc mstatus,8" )
// #define enable_interrupts()	__asm volatile  ( "csrs mstatus,8" )

#define enable_timer_interrupts()	set_csr_bit(mie, MIE_MTIE)
#define disable_timer_interrupts()	clear_csr_bit(mie, MIE_MTIE)

#define enable_external_interrupts()	set_csr_bit(mie, MIE_MEIE)
#define disable_external_interrupts()	clear_csr_bit(mie, MIE_MEIE)

#define enable_software_interrupts()	set_csr_bit(mie, MIE_MSIE)
#define disable_software_interrupts()	clear_csr_bit(mie, MIE_MSIE)


#define RISCV_TIMER_IRQ_DEF(timer_handler, pclock_source) \
	int (*__riscv64_timer_handler)(unsigned int, void *) = timer_handler; \
	void *__riscv64_timer_data = pclock_source;

extern int (*__riscv64_timer_handler)(unsigned int, void *);
extern void *__riscv64_timer_data;

#endif /* RISCV_INTERRUPTS_H_ */
