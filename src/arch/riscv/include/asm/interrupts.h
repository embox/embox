#ifndef RISCV_INTERRUPTS_H_
#define RISCV_INTERRUPTS_H_

#include <asm/regs.h>

#define disable_interrupts() clear_csr_bit(STATUS_REG, STATUS(IE))
#define enable_interrupts()  set_csr_bit(STATUS_REG, STATUS(IE))

// #define disable_interrupts()	__asm volatile  ( "csrc mstatus,8" )
// #define enable_interrupts()	__asm volatile  ( "csrs mstatus,8" )

#define enable_timer_interrupts()  set_csr_bit(INTERRUPT_REG, IE(TIE))
#define disable_timer_interrupts() clear_csr_bit(INTERRUPT_REG, IE(TIE))

#define enable_external_interrupts()  set_csr_bit(INTERRUPT_REG, IE(EIE))
#define disable_external_interrupts() clear_csr_bit(INTERRUPT_REG, IE(EIE))

#define enable_software_interrupts()  set_csr_bit(INTERRUPT_REG, IE(SIE))
#define disable_software_interrupts() clear_csr_bit(INTERRUPT_REG, IE(SIE))

#define RISCV_TIMER_IRQ_DEF(timer_handler, pclock_source)               \
	int (*__riscv_timer_handler)(unsigned int, void *) = timer_handler; \
	void *__riscv_timer_data = pclock_source;

extern int (*__riscv_timer_handler)(unsigned int, void *) __attribute__((weak));
extern void *__riscv_timer_data __attribute__((weak));

#endif /* RISCV_INTERRUPTS_H_ */
