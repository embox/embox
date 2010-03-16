/**
 * @file
 * @brief TODO
 *
 * @date 16.03.2010
 * @author Eldar Abusalimov
 */

#include <asm/regs.h>

/* scratch locals used in trap context. */
#define t0      t_twinmask
#define t1      temp
#define t2      local

/* current window pointer     */
#define PSR_CWP     0x0000001f
/* enable traps field         */
#define PSR_ET      0x00000020
/* previous privilege level   */
#define PSR_PS      0x00000040
/* current privilege level    */
#define PSR_S       0x00000080
/* processor interrupt level  */
#define PSR_PIL     0x00000f00
/* enable floating point      */
#define PSR_EF      0x00001000
/* enable co-processor        */
#define PSR_EC      0x00002000
/* SuperSparcII little-endian */
#define PSR_LE      0x00008000
/* integer condition codes    */
#define PSR_ICC     0x00f00000
/* carry bit                  */
#define PSR_C       0x00100000
/* overflow bit               */
#define PSR_V       0x00200000
/* zero bit                   */
#define PSR_Z       0x00400000
/* negative bit               */
#define PSR_N       0x00800000
/* cpu-version field          */
#define PSR_VERS    0x0f000000
/* cpu-implementation field   */
#define PSR_IMPL    0xf0000000

/* zero (alignment)           */
#define TBR_ZERO    0x0000000f
/* trap type field            */
#define TBR_TT      0x00000ff0
/* trap base address          */
#define TBR_TBA     0xfffff000

#define PSR_BIT_TOGGLE_SHORT(mask, scratch_psr) \
	rd %psr, %scratch_psr;                       \
	wr %scratch_psr, mask, %psr;

#define PSR_BIT_TOGGLE(mask, scratch_psr, scratch_mask) \
	rd  %psr, %scratch_psr;                        \
	set mask, %scratch_mask;                       \
	wr  %scratch_psr, %scratch_mask, %psr;

#define PSR_BIT_SET_SHORT(mask, scratch_psr) \
	rd  %psr, %scratch_psr;                   \
	or  %scratch_psr, mask, %scratch_psr;     \
	wr  %scratch_psr, %g0, %psr;

#define PSR_BIT_SET(mask, scratch_psr, scratch_mask) \
	rd  %psr, %scratch_psr;                        \
	set mask, %scratch_mask;                       \
	or  %scratch_psr, %scratch_mask, %scratch_psr; \
	wr  %scratch_psr, %g0, %psr;

#define PSR_BIT_CLEAR_SHORT(mask, scratch_psr) \
	rd   %psr, %scratch_psr;                    \
	andn %scratch_psr, mask, %scratch_psr;      \
	wr   %scratch_psr, %g0, %psr;

#define PSR_BIT_CLEAR(mask, scratch_psr, scratch_mask) \
	rd   %psr, %scratch_psr;                       \
	set  mask, %scratch_mask;                      \
	andn %scratch_psr, %scratch_mask, %scratch_psr;\
	wr   %scratch_psr, %g0, %psr;

#define PSR_BIT_COPY(r_src, mask, scratch_psr, scratch_src) \
	rd   %psr, %scratch_psr;                       \
	set  mask, %scratch_src;                       \
	andn %scratch_psr, %scratch_src, %scratch_psr; \
	and  %r_src, %scratch_src, %scratch_src;       \
	or   %scratch_psr, %scratch_src, %scratch_psr; \
	wr   %scratch_psr, %g0, %psr;

/* check whether WIM[CWP] == 1 or not */
#define PSR_CWP_WIM_TEST(r_psr, r_wim, scratch) \
	srl %r_wim, %r_psr, %scratch;                \
	andcc %scratch, 0x1, %g0;

/** Circular right shifting by 1 bit */
#define WIM_SHIFT_RIGHT(r_oldwim, r_newwim, scratch) \
	srl %r_oldwim, 1, %r_newwim;                   \
	sll %r_oldwim, CONFIG_NWINDOWS - 1, %scratch;    \
	or %r_newwim, %scratch, %r_newwim;

/** Circular left shifting by 1 bit */
#define WIM_SHIFT_LEFT(r_oldwim, r_newwim, scratch)  \
	sll %r_oldwim, 1, %r_newwim;                   \
	srl %r_oldwim, CONFIG_NWINDOWS - 1, %scratch;    \
	or %r_newwim, %scratch, %r_newwim;

/** compute sizes by hand */
#define REG_WINDOW_SZ ((8+8)*4)

/**
 * REG_WINDOW offsets
 */
#define RW_L0     0x00
#define RW_L1     0x04
#define RW_L2     0x08
#define RW_L3     0x0c
#define RW_L4     0x10
#define RW_L5     0x14
#define RW_L6     0x18
#define RW_L7     0x1c
#define RW_I0     0x20
#define RW_I1     0x24
#define RW_I2     0x28
#define RW_I3     0x2c
#define RW_I4     0x30
#define RW_I5     0x34
#define RW_I6     0x38
#define RW_I7     0x3c

/**
 * Store the register window onto the 8-byte aligned area starting at %reg.
 */
#define STORE_WINDOW(reg, offset) \
	std	%l0, [%reg + (offset) + RW_L0]; \
	std	%l2, [%reg + (offset) + RW_L2]; \
	std	%l4, [%reg + (offset) + RW_L4]; \
	std	%l6, [%reg + (offset) + RW_L6]; \
	std	%i0, [%reg + (offset) + RW_I0]; \
	std	%i2, [%reg + (offset) + RW_I2]; \
	std	%i4, [%reg + (offset) + RW_I4]; \
	std	%i6, [%reg + (offset) + RW_I6];

/**
 * Load a register window from the 8-byte aligned area beginning at %reg.
 */
#define LOAD_WINDOW(reg, offset) \
	ldd	[%reg + (offset) + RW_L0], %l0; \
	ldd	[%reg + (offset) + RW_L2], %l2; \
	ldd	[%reg + (offset) + RW_L4], %l4; \
	ldd	[%reg + (offset) + RW_L6], %l6; \
	ldd	[%reg + (offset) + RW_I0], %i0; \
	ldd	[%reg + (offset) + RW_I2], %i2; \
	ldd	[%reg + (offset) + RW_I4], %i4; \
	ldd	[%reg + (offset) + RW_I6], %i6;


/* compute sizes by hand (see above) */
#define STACKFRAME_SZ     (REG_WINDOW_SZ + (1+6+1)*4)
#define TRAP_CONTEXT_SZ   ((1+1+1+1+8+8)*4)

/* Offsets for trap context structure  */
#define PT_PSR    0x0
#define PT_PC     0x4
#define PT_NPC    0x8
#define PT_Y      0xc
#define PT_G0     0x10
#define PT_G1     0x14
#define PT_G2     0x18
#define PT_G3     0x1c
#define PT_G4     0x20
#define PT_G5     0x24
#define PT_G6     0x28
#define PT_G7     0x2c
#define PT_I0     0x30
#define PT_I1     0x34
#define PT_I2     0x38
#define PT_I3     0x3c
#define PT_I4     0x40
#define PT_I5     0x44
#define PT_I6     0x48
#define PT_I7     0x4c

#define LOAD_PT_GLOBALS(base_reg) \
	ld      [%base_reg + STACKFRAME_SZ + PT_G1], %g1; \
	ldd     [%base_reg + STACKFRAME_SZ + PT_G2], %g2; \
	ldd     [%base_reg + STACKFRAME_SZ + PT_G4], %g4; \
	ldd     [%base_reg + STACKFRAME_SZ + PT_G6], %g6;

#define LOAD_PT_INS(base_reg) \
	ldd [%base_reg + STACKFRAME_SZ + PT_I0], %i0; \
	ldd [%base_reg + STACKFRAME_SZ + PT_I2], %i2; \
	ldd [%base_reg + STACKFRAME_SZ + PT_I4], %i4; \
	ldd [%base_reg + STACKFRAME_SZ + PT_I6], %i6;

#define LOAD_PT_YREG(base_reg, scratch) \
	ld      [%base_reg + STACKFRAME_SZ + PT_Y], %scratch; \
	wr      %scratch, 0x0, %y;

#define LOAD_PT_PRIV(base_reg, pt_psr, pt_pc, pt_npc) \
	ld      [%base_reg + STACKFRAME_SZ + PT_PSR], %pt_psr; \
	ld      [%base_reg + STACKFRAME_SZ + PT_PC], %pt_pc; \
	ld      [%base_reg + STACKFRAME_SZ + PT_NPC], %pt_npc;

#define LOAD_PT_ALL(base_reg, pt_psr, pt_pc, pt_npc, g_scratch) \
	LOAD_PT_YREG(base_reg, g_scratch) \
	LOAD_PT_GLOBALS(base_reg) \
	LOAD_PT_INS(base_reg) \
	LOAD_PT_PRIV(base_reg, pt_psr, pt_pc, pt_npc)

#define STORE_PT_GLOBALS(base_reg) \
	st      %g1, [%base_reg + STACKFRAME_SZ + PT_G1]; \
	std     %g2, [%base_reg + STACKFRAME_SZ + PT_G2]; \
	std     %g4, [%base_reg + STACKFRAME_SZ + PT_G4]; \
	std     %g6, [%base_reg + STACKFRAME_SZ + PT_G6];

#define STORE_PT_INS(base_reg) \
	std %i0, [%base_reg + STACKFRAME_SZ + PT_I0]; \
	std %i2, [%base_reg + STACKFRAME_SZ + PT_I2]; \
	std %i4, [%base_reg + STACKFRAME_SZ + PT_I4]; \
	std %i6, [%base_reg + STACKFRAME_SZ + PT_I6];

#define STORE_PT_YREG(base_reg, scratch) \
	rd      %y, %scratch; \
	st      %scratch, [%base_reg + STACKFRAME_SZ + PT_Y];

#define STORE_PT_PRIV(base_reg, pt_psr, pt_pc, pt_npc) \
	st      %pt_psr, [%base_reg + STACKFRAME_SZ + PT_PSR]; \
	st      %pt_pc,  [%base_reg + STACKFRAME_SZ + PT_PC]; \
	st      %pt_npc, [%base_reg + STACKFRAME_SZ + PT_NPC];

#define STORE_PT_ALL(base_reg, reg_psr, reg_pc, reg_npc, g_scratch) \
	STORE_PT_PRIV(base_reg, reg_psr, reg_pc, reg_npc) \
	STORE_PT_INS(base_reg) \
	STORE_PT_GLOBALS(base_reg) \
	STORE_PT_YREG(base_reg, g_scratch) \

#define SAVE_ALL \
	sethi %hi(trap_setup_begin), %t_retpc;            \
	jmpl  %t_retpc + %lo(trap_setup_begin), %t_retpc; \
	 rd %wim, %t_wim;

#define RESTORE_ALL \
	sethi %hi(trap_setup_end), %t_retpc;       \
	jmpl  %t_retpc + %lo(trap_setup_end), %g0; \
	 nop;

	.section ".text"
	.align 4


/* Window overflow handling routine. */
		.global window_overflow
window_overflow:

#define g_newwim g1

	/* Remember previous value of %g_newwim to restore it later */
	mov %g_newwim, %t0

	/* Calculate new WIM */
	WIM_SHIFT_RIGHT(t_wim, g_newwim, t1)
	/*
	 * at this line WIM points to the current window
	 * so SAVE wouldn't not cause any trap
	 */
	save	// Get into the window to be saved

	mov %g_newwim, %wim	// Write new WIM value that has been just calculated
	 // do not wait for delayed write

	STORE_WINDOW(sp, 0)

	/*
	 * at this line WIM points to the current window again
	 * so RESTORE wouldn't not cause any trap
	 */
	restore	// Go back to trap window

	/* Restore saved %g_newwim */
	mov %t0, %g_newwim

	jmp %t_pc	// Re-execute SAVE
	 rett %t_npc


/* Window underflow handling routine.  */
		.global  window_underflow
window_underflow:

#define l_newwim t0

	/* Calculate new WIM */
	WIM_SHIFT_LEFT(t_wim, l_newwim, t1)

	mov	%l_newwim, %wim	// Write new WIM value
	 nop; nop; nop	// Wait for WIM delayed writing

	/*
	 * Two restores to get into the window to be restored
	 */
	restore
	restore

	LOAD_WINDOW(sp, 0)

	save
	save	// Get back to trap window

	jmp  %l1	// Re-execute RESTORE
	 rett  %l2


/* rounds "x" up to doubleword boundary */
#define SA(x) (((x)+7)&(~0x07))
/* trap frame size */
#define TRAPFRAME_SZ (STACKFRAME_SZ + TRAP_CONTEXT_SZ)

#define t_sp t2

	.global trap_setup_begin
trap_setup_begin:
	/*
	 * First of all, we allocate a new stack frame,
	 * and write the cpu state (globals, psr, pc, npc, y, ...) to it.
	 *
	 * Note that we do not update the real sp register,
	 * we'll do it a bit later after cheking for possible window overflow.
	 * (we use local t_sp instead,
	 * be careful and do not change it until moving to the real sp)
	 */

	add %fp, -SA(TRAPFRAME_SZ), %t_sp
	STORE_PT_ALL(t_sp, t_psr, t_pc, t_npc, t0)

	/*
	 * Traps are still disabled at this line,
	 * and the cpu state has just been stored in the memory.
	 * Therefore we can quietly use globals until reenabling traps
	 * and doing the callback into an arbitrary code.
	 */

	/* test CWP for possible window overflow occurred when entering trap */
	PSR_CWP_WIM_TEST(t_psr, t_wim, t0)

	bz trap_wof_test_ok
	 nop/*
		 * Handle window overflow.
		 * Just do the same as the corresponding trap handler routine.
		 * (See windows.S)
		 */

		// Calculate new WIM
		WIM_SHIFT_RIGHT(t_wim, g1, t0)

		save	// Get into the window to be saved

		wr %g1, %g0, %wim
		 // do not wait for delayed write

		STORE_WINDOW(sp, 0)

		restore	// Go back to trap window

	trap_wof_test_ok:
	/*
	 * OK, return to handler and update the real stack pointer
	 */
	jmpl %t_retpc + 8, %g0
	 mov %t_sp, %sp


	.global trap_setup_end
trap_setup_end:
	/*
	 * We do not need trap-time WIM initial value anymore.
	 * So read _new_ value of WIM to %t_wim
	 * (just for code readability).
	 */
	rd %wim, %t_wim;

	/* Now we need to check if the RETT would land us in an invalid window */
	WIM_SHIFT_RIGHT(t_wim, g2, t0)
	/*
	 * we are in the same window as at trap entering time
	 * so get CWP from an old local copy of PSR
	 */
	PSR_CWP_WIM_TEST(t_psr, g2, t0)

	bz trap_wuf_test_ok
	 nop/*
		 * Handle window underflow.
		 * Just do the same as the corresponding trap handler routine.
		 * (See windows.S)
		 */

		/* Calculate new WIM */
		WIM_SHIFT_LEFT(t_wim, g1, t0)

		wr %g1, %g0, %wim
		 WRITE_PAUSE	// wait for delayed-write before RESTORE

		restore	// get into the window to be restored

		LOAD_WINDOW(sp, 0)

		save	// Get back to the trap window

	trap_wuf_test_ok:
	/*
	 * OK, now we feel sure that RETT will not cause any trap
	 */

	LOAD_PT_ALL(sp, t_psr, t_pc, t_npc, t0)

	/*
	 * Be careful here, do not damage context (globals in particular)
	 * that has been just restored.
	 */

// TODO what's about checking and restoring PS/S? -- Eldar
	/* restore PIL&ICC fields of old PSR */
	PSR_BIT_COPY(t_psr, (PSR_PIL | PSR_ICC), t0, t1)
	 WRITE_PAUSE

	/* tata! */
	jmp %t_pc
	 rett %t_npc


trap_dispatcher:
	! store previous nPC as PC
	mov		%t_npc, %t_pc
	! store previous nPC+4 as nPC
	add     %t_npc, 4, %t_npc


	.global weak_trap_entry
weak_trap_entry:
	orcc %local, %g0, %g0
	be bad_trap_entry
	 nop
	jmpl %local, %g0
	 nop

 	.global srmmu_fault
srmmu_fault:
!	mov %t_npc, %t_pc
!	add %t_npc, 4, %t_npc
	jmpl %t_npc, %g0
	 rett %t_npc + 4

	.global interrupt_entry
interrupt_entry:
irq_dispatcher:
interrupting_trap:

	SAVE_ALL

	mov %sp, %o1

	// pass trap type as parameter
	mov %tbr, %o0
	and %o0, TBR_TT, %o0
	srl %o0, 4, %o0

	/*
	 * enable traps
	 */
	PSR_BIT_TOGGLE_SHORT(PSR_ET, t0)
	 // do not wait (PSR_ET)

		call	dispatch_trap
		 nop

	/*
	 * disable traps
	 *
	 * and do not worry that the a�?RDPSR, WRPSRa�? sequence is interruptible,
	 * our interrupts dispatcher does not corrupt PSR
	 * and restores it to the initial value before returning from trap
	 * (see below)
	 */
	PSR_BIT_TOGGLE_SHORT(PSR_ET, t0)
	 // do not wait (PSR_ET)

	RESTORE_ALL

	.global bad_trap_entry
bad_trap_entry:
bad_trap_dispatcher:
	SAVE_ALL

	mov %tbr, %o1
	srl %o1, 4, %o1
	and %o1, 0xFF, %o1		! pass TT as parameter

	PSR_BIT_SET_SHORT(PSR_PIL, t0)
	 WRITE_PAUSE
	PSR_BIT_TOGGLE_SHORT(PSR_ET, t0)
	 WRITE_PAUSE

!	call dispatch_bad_trap
	 add %sp, STACKFRAME_SZ, %o0

0:	ba 0b
	 nop

	RESTORE_ALL
