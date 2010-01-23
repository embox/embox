/**
 * @file
 * @brief Defines macros used to construct the trap table.
 *
 * @date 09.06.2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_HEAD_H_
#define SPARC_HEAD_H_

#include <asm/reg_alias.h>

#ifdef __ASSEMBLER__

/** Entry for traps which jump to a programmer-specified trap handler.  */
#define TRAP_ENTRY(trap_handler) \
	ba trap_handler; \
	 rd %psr, %t_psr;\
	nop; nop;

/** Entry for traps which jump to a programmer-specified trap handler.  */
#define TRAP_ENTRY_INTERRUPT TRAP_ENTRY(interrupt_entry)

/** Entry point for window overflow trap handler. */
#define WOF_TRAP \
	rd %psr, %t_psr;      \
	ba window_overflow;   \
	 rd %wim, %t_wim;     \
	nop

/** Entry point for window underflow trap handler. */
#define WUF_TRAP \
	rd %psr, %t_psr;      \
	ba window_underflow;  \
	 rd %wim, %t_wim;     \
	nop

/** Text fault. */
#define SRMMU_TFAULT rd %psr, %l0; rd %wim, %l3; b srmmu_fault; mov 1, %l7;
/** Data fault. */
#define SRMMU_DFAULT rd %psr, %l0; rd %wim, %l3; b srmmu_fault; mov 0, %l7;

/** Unexpected trap will halt the processor by forcing it to error state */
#define BAD_TRAP TRAP_ENTRY(bad_trap_dispatcher)

/** Software trap. Treat as BAD_TRAP */
#define SOFT_TRAP BAD_TRAP

#endif /* __ASSEMBLER__ */

#endif /* SPARC_HEAD_H_ */
