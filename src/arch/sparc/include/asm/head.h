/**
 * @file
 * @brief Defines macros used to construct the trap table.
 *
 * @date 09.06.09
 * @author Eldar Abusalimov
 */

#ifndef SPARC_HEAD_H_
#define SPARC_HEAD_H_

#include <asm/regs.h>
#include <module/embox/arch/interrupt.h>
#include <module/embox/arch/syscall.h>

#ifdef __ASSEMBLER__

/** Entry for traps which jump to a programmer-specified trap handler. */
#define TRAP_ENTRY(trap_handler) \
	ba trap_handler; \
	 rd %psr, %t_psr;\
	nop; nop;

/** Entry for trap handlers that skip the instruction caused the trap. */
#define TRAP_ENTRY_SKIP(trap_handler) \
	mov %t_npc, %t_pc;     \
	add %t_npc, 4, %t_npc; \
	ba trap_handler;       \
	 rd %psr, %t_psr;

#ifndef INTERRUPT_STUB

/** Entry for interrupting traps. */
#define TRAP_ENTRY_INTERRUPT(nr) \
	rd %psr, %t_psr;    \
	ba interrupt_entry; \
	 mov nr, %local;    \
	nop;

#else
#define TRAP_ENTRY_INTERRUPT(nr) BAD_TRAP
#endif /* INTERRUPT_STUB */

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

#define WEAK(name) \
	.weak hw_trap_##name;                  \
	sethi %hi(hw_trap_##name), %local;     \
	or %local,%lo(hw_trap_##name), %local; \
	ba weak_trap_entry;                    \
	 rd %psr, %t_psr;

#ifdef CONFIG_MMU_SUPPORT
/** Text fault. */
#define SRMMU_TFAULT \
	rd %psr, %l0;    \
	rd %wim, %l3;    \
	b srmmu_fault;   \
	mov 1, %local;

/** Data fault. */
#define SRMMU_DFAULT \
	rd %psr, %l0;    \
	rd %wim, %l3;    \
	b srmmu_fault;   \
	mov 0, %local;
#else
#define SRMMU_TFAULT BAD_TRAP
#define SRMMU_DFAULT BAD_TRAP
#endif

/** Unexpected trap will halt the processor by forcing it to error state */
#define BAD_TRAP TRAP_ENTRY(bad_trap_entry)

/** Software trap. Treat as BAD_TRAP */
#define SOFT_TRAP BAD_TRAP

#ifndef SYSCALL_STUB
#define EMBOX_SYSTEM_CALL TRAP_ENTRY(syscall_entry)
#else
#define EMBOX_SYSTEM_CALL BAD_TRAP
#endif


#define TEST_SOFT_TRAP TRAP_ENTRY(test_trap_entry)
#define TEST_HARD_TRAP TEST_SOFT_TRAP

#endif /* __ASSEMBLER__ */

#endif /* SPARC_HEAD_H_ */
