/**
 * @file
 *
 * @date 25.11.09
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_TRAPS_H_
#define MICROBLAZE_TRAPS_H_

#ifndef __ASSEMBLER__

#else /* __ASSEMBLER__ */

/*
 * Holds PSR value.
 * Written each time when trap occurs
 * (first instruction of most trap entries).
 */
#define t_psr   l0
/* PC to return from trap. Filled by the hardware. */
#define t_pc    l1
/* nPC to return from trap. Filled by the hardware. */
#define t_npc   l2
/* WIM is written each time when trap occurs. */
#define t_wim   l3
/* local register used in trap context as return address instead of i7/o7. */
#define t_retpc l7

/* scratch locals used in trap context. */
#define t0      l4
#define t1      l5
#define t2      l6

/* Entry for traps which jump to a programmer-specified trap handler.  */
#define TRAP(H) \
	mov %psr, %t_psr;  \
	sethi %hi(H), %t0; \
	jmp %t0 + %lo(H);  \
	mov %wim, %t_wim;

#define TRAPL(H) \
	mov %g0, %t_psr;   \
	sethi %hi(H), %t0; \
	jmp %t0 + %lo(H);  \
	mov %wim, %t_wim;

/* Unexpected trap will halt the processor by forcing it to error state */
#define BAD_TRAP TRAP(bad_trap_dispatcher)

/* Software trap. Treat as BAD_TRAP */
#define SOFT_TRAP BAD_TRAP

/*for mmu*/

#define SAVE_ALL \
	sethi %hi(trap_entry_begin), %t_retpc;            \
	jmpl  %t_retpc + %lo(trap_entry_begin), %t_retpc; \
	nop;

#define RESTORE_ALL \
	sethi %hi(trap_entry_end), %t_retpc;       \
	jmpl  %t_retpc + %lo(trap_entry_end), %g0; \
	nop;

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
	ld      [%base_reg + STACKFRAME_SZ + PT_PC], %pt_pc;   \
	ld      [%base_reg + STACKFRAME_SZ + PT_NPC], %pt_npc;

#define LOAD_PT_ALL(base_reg, pt_psr, pt_pc, pt_npc, g_scratch) \
	LOAD_PT_YREG(base_reg, g_scratch) \
	LOAD_PT_GLOBALS(base_reg)         \
	LOAD_PT_INS(base_reg)             \
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
	rd      %y, %scratch;            \
	st      %scratch, [%base_reg + STACKFRAME_SZ + PT_Y];

#define STORE_PT_PRIV(base_reg, pt_psr, pt_pc, pt_npc) \
	st      %pt_psr, [%base_reg + STACKFRAME_SZ + PT_PSR]; \
	st      %pt_pc,  [%base_reg + STACKFRAME_SZ + PT_PC];  \
	st      %pt_npc, [%base_reg + STACKFRAME_SZ + PT_NPC];

#define STORE_PT_ALL(base_reg, reg_psr, reg_pc, reg_npc, g_scratch) \
	STORE_PT_PRIV(base_reg, reg_psr, reg_pc, reg_npc) \
	STORE_PT_INS(base_reg)             \
	STORE_PT_GLOBALS(base_reg)         \
	STORE_PT_YREG(base_reg, g_scratch) \

#endif /* __ASSEMBLER__ */

#endif /* MICROBLAZE_TRAPS_H_ */
