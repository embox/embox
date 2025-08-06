#ifndef RISCV_INTERRUPTS_H_
#define RISCV_INTERRUPTS_H_

#include <asm/csr.h>

#define disable_interrupts() csr_clear(CSR_STATUS, CSR_STATUS_IE)
#define enable_interrupts()  csr_set(CSR_STATUS, CSR_STATUS_IE)

// #define disable_interrupts()	__asm volatile  ( "csrc mstatus,8" )
// #define enable_interrupts()	__asm volatile  ( "csrs mstatus,8" )

#define enable_timer_interrupts()  csr_set(CSR_IE, CSR_IE_TIE)
#define disable_timer_interrupts() csr_clear(CSR_IE, CSR_IE_TIE)

#define enable_external_interrupts()  csr_set(CSR_IE, CSR_IE_EIE)
#define disable_external_interrupts() csr_clear(CSR_IE, CSR_IE_EIE)

#define enable_software_interrupts()  csr_set(CSR_IE, CSR_IE_SIE)
#define disable_software_interrupts() csr_clear(CSR_IE, CSR_IE_SIE)

#endif /* RISCV_INTERRUPTS_H_ */
