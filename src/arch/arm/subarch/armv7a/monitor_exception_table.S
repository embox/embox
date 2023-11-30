/**
 * @file
 * @brief
 *
 * @author Anton Kozlov
 * @date 10.09.2012
 */

#include <asm/regs.h>
#include <framework/mod/options.h>

.pushsection .monitor_trap_table, "ax"

.extern GLOBAL_LR1
.extern GLOBAL_SP1
.extern GLOBAL_CPSR1
.extern GLOBAL_LR2
.extern GLOBAL_SP2
.extern GLOBAL_CPSR2

.align 5
/* the vector table for secure state and HYP mode */
.global _monitor_vectors
_monitor_vectors:
	adr     pc, _secure_undef_exception
	adr     pc, _secure_undef_exception
	adr     pc, _secure_monitor
	adr     pc, _secure_undef_exception
	adr     pc, _secure_undef_exception
	adr     pc, _secure_undef_exception
	adr     pc, _secure_undef_exception
	adr     pc, _secure_undef_exception

_secure_undef_exception:
	nop

_secure_monitor:

	ldr     r1, =GLOBAL_LR1
	str     r14, [r1]

	ldr     r1, =GLOBAL_SP1
	str     sp, [r1]

	ldr     r1, =GLOBAL_CPSR1
	mrs     r0, CPSR
	str     r0, [r1]

	ldr     r1, =GLOBAL_SPSR1
	mrs     r0, SPSR
	str     r0, [r1]

#bl mon_func

	mrc     p15, 0, r10, c1, c1, 0
	bic     r10, r10, #0x4e @ clear IRQ, FIQ, EA, nET bits
	orr     r10, r10, #0x31 @ enable NS, AW, FW bits
	mcr     p15, 0, r10, c1, c1, 0
	isb

	ldr     r1, =GLOBAL_LR2
	str     r14, [r1]

	ldr     r1, =GLOBAL_SP2
	str     sp, [r1]

	ldr     r1, =GLOBAL_CPSR2
	mrs     r0, CPSR
	str     r0, [r1]

	ldr     r1, =GLOBAL_SPSR2
	mrs     r0, SPSR
	str     r0, [r1]

	mrc     p15, 0, r10, c1, c1, 0
	bic     r10, r10, #0x4e @ clear IRQ, FIQ, EA, nET bits
	bic     r10, r10, #0x01 @ enable NS, AW, FW bits
	mcr     p15, 0, r10, c1, c1, 0
	isb

	movs    pc, lr

.popsection
