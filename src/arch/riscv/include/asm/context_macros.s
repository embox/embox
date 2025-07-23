/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 18.07.25
 */

#ifndef RISCV_ASM_CONTEXT_H_
#error "Do not include this file directly!"
#endif

/* Offsets of caller-saved X registers */
#define __caller_saved_a0_offset   (REG_SIZE_X * 0)
#define __caller_saved_a1_offset   (REG_SIZE_X * 1)
#define __caller_saved_a2_offset   (REG_SIZE_X * 2)
#define __caller_saved_a3_offset   (REG_SIZE_X * 3)
#define __caller_saved_a4_offset   (REG_SIZE_X * 4)
#define __caller_saved_a5_offset   (REG_SIZE_X * 5)
#define __caller_saved_a6_offset   (REG_SIZE_X * 6)
#define __caller_saved_a7_offset   (REG_SIZE_X * 7)
#define __caller_saved_t0_offset   (REG_SIZE_X * 8)
#define __caller_saved_t1_offset   (REG_SIZE_X * 9)
#define __caller_saved_t2_offset   (REG_SIZE_X * 10)
#define __caller_saved_t4_offset   (REG_SIZE_X * 11)
#define __caller_saved_t3_offset   (REG_SIZE_X * 12)
#define __caller_saved_t5_offset   (REG_SIZE_X * 13)
#define __caller_saved_t6_offset   (REG_SIZE_X * 14)

/* Offsets of callee-saved X registers */
#define __callee_saved_ra_offset   (REG_SIZE_X * 0)
#define __callee_saved_sp_offset   (REG_SIZE_X * 1)
#define __callee_saved_s0_offset   (REG_SIZE_X * 2)
#define __callee_saved_s1_offset   (REG_SIZE_X * 3)
#define __callee_saved_s2_offset   (REG_SIZE_X * 4)
#define __callee_saved_s3_offset   (REG_SIZE_X * 5)
#define __callee_saved_s4_offset   (REG_SIZE_X * 6)
#define __callee_saved_s5_offset   (REG_SIZE_X * 7)
#define __callee_saved_s6_offset   (REG_SIZE_X * 8)
#define __callee_saved_s7_offset   (REG_SIZE_X * 9)
#define __callee_saved_s8_offset   (REG_SIZE_X * 10)
#define __callee_saved_s9_offset   (REG_SIZE_X * 11)
#define __callee_saved_s10_offset  (REG_SIZE_X * 12)
#define __callee_saved_s11_offset  (REG_SIZE_X * 13)

/* Offsets of caller-saved F registers */
#define __caller_saved_fa0_offset  (REG_SIZE_F * 0)
#define __caller_saved_fa1_offset  (REG_SIZE_F * 1)
#define __caller_saved_fa2_offset  (REG_SIZE_F * 2)
#define __caller_saved_fa3_offset  (REG_SIZE_F * 3)
#define __caller_saved_fa4_offset  (REG_SIZE_F * 4)
#define __caller_saved_fa5_offset  (REG_SIZE_F * 5)
#define __caller_saved_fa6_offset  (REG_SIZE_F * 6)
#define __caller_saved_fa7_offset  (REG_SIZE_F * 7)
#define __caller_saved_ft0_offset  (REG_SIZE_F * 8)
#define __caller_saved_ft1_offset  (REG_SIZE_F * 9)
#define __caller_saved_ft2_offset  (REG_SIZE_F * 10)
#define __caller_saved_ft3_offset  (REG_SIZE_F * 11)
#define __caller_saved_ft4_offset  (REG_SIZE_F * 12)
#define __caller_saved_ft5_offset  (REG_SIZE_F * 13)
#define __caller_saved_ft6_offset  (REG_SIZE_F * 14)
#define __caller_saved_ft7_offset  (REG_SIZE_F * 15)
#define __caller_saved_ft8_offset  (REG_SIZE_F * 16)
#define __caller_saved_ft9_offset  (REG_SIZE_F * 17)
#define __caller_saved_ft10_offset (REG_SIZE_F * 18)
#define __caller_saved_ft11_offset (REG_SIZE_F * 19)

/* Offsets of callee-saved F registers */
#define __callee_saved_fs0_offset  (REG_SIZE_F * 0)
#define __callee_saved_fs1_offset  (REG_SIZE_F * 1)
#define __callee_saved_fs2_offset  (REG_SIZE_F * 2)
#define __callee_saved_fs3_offset  (REG_SIZE_F * 3)
#define __callee_saved_fs4_offset  (REG_SIZE_F * 4)
#define __callee_saved_fs5_offset  (REG_SIZE_F * 5)
#define __callee_saved_fs6_offset  (REG_SIZE_F * 6)
#define __callee_saved_fs7_offset  (REG_SIZE_F * 7)
#define __callee_saved_fs8_offset  (REG_SIZE_F * 8)
#define __callee_saved_fs9_offset  (REG_SIZE_F * 9)
#define __callee_saved_fs10_offset (REG_SIZE_F * 10)
#define __callee_saved_fs11_offset (REG_SIZE_F * 11)

.macro __do_caller_saved_x op, base, offset
	\op     a0, (\offset + __caller_saved_a0_offset)(\base)
	\op     a1, (\offset + __caller_saved_a1_offset)(\base)
	\op     a2, (\offset + __caller_saved_a2_offset)(\base)
	\op     a3, (\offset + __caller_saved_a3_offset)(\base)
	\op     a4, (\offset + __caller_saved_a4_offset)(\base)
	\op     a5, (\offset + __caller_saved_a5_offset)(\base)
	\op     a6, (\offset + __caller_saved_a6_offset)(\base)
	\op     a6, (\offset + __caller_saved_a7_offset)(\base)
	\op     t0, (\offset + __caller_saved_t0_offset)(\base)
	\op     t1, (\offset + __caller_saved_t1_offset)(\base)
	\op     t2, (\offset + __caller_saved_t2_offset)(\base)
	\op     t3, (\offset + __caller_saved_t4_offset)(\base)
	\op     t4, (\offset + __caller_saved_t3_offset)(\base)
	\op     t5, (\offset + __caller_saved_t5_offset)(\base)
	\op     t6, (\offset + __caller_saved_t6_offset)(\base)
.endm

.macro __do_callee_saved_x op, base, offset
	\op     ra, (\offset + __callee_saved_ra_offset)(\base)
	\op     sp, (\offset + __callee_saved_sp_offset)(\base)
	\op     s0, (\offset + __callee_saved_s0_offset)(\base)
	\op     s1, (\offset + __callee_saved_s1_offset)(\base)
	\op     s2, (\offset + __callee_saved_s2_offset)(\base)
	\op     s3, (\offset + __callee_saved_s3_offset)(\base)
	\op     s4, (\offset + __callee_saved_s4_offset)(\base)
	\op     s5, (\offset + __callee_saved_s5_offset)(\base)
	\op     s6, (\offset + __callee_saved_s6_offset)(\base)
	\op     s7, (\offset + __callee_saved_s7_offset)(\base)
	\op     s8, (\offset + __callee_saved_s8_offset)(\base)
	\op     s9, (\offset + __callee_saved_s9_offset)(\base)
	\op     s10, (\offset + __callee_saved_s10_offset)(\base)
	\op     s11, (\offset + __callee_saved_s11_offset)(\base)
.endm

.macro __do_caller_saved_f op, base, offset
	\op     fa0, (\offset + __caller_saved_fa0_offset)(\base)
	\op     fa1, (\offset + __caller_saved_fa1_offset)(\base)
	\op     fa2, (\offset + __caller_saved_fa2_offset)(\base)
	\op     fa3, (\offset + __caller_saved_fa3_offset)(\base)
	\op     fa4, (\offset + __caller_saved_fa4_offset)(\base)
	\op     fa5, (\offset + __caller_saved_fa5_offset)(\base)
	\op     fa6, (\offset + __caller_saved_fa6_offset)(\base)
	\op     fa7, (\offset + __caller_saved_fa7_offset)(\base)
	\op     ft0, (\offset + __caller_saved_ft0_offset)(\base)
	\op     ft1, (\offset + __caller_saved_ft1_offset)(\base)
	\op     ft2, (\offset + __caller_saved_ft2_offset)(\base)
	\op     ft3, (\offset + __caller_saved_ft3_offset)(\base)
	\op     ft4, (\offset + __caller_saved_ft4_offset)(\base)
	\op     ft5, (\offset + __caller_saved_ft5_offset)(\base)
	\op     ft6, (\offset + __caller_saved_ft6_offset)(\base)
	\op     ft7, (\offset + __caller_saved_ft7_offset)(\base)
	\op     ft8, (\offset + __caller_saved_ft8_offset)(\base)
	\op     ft9, (\offset + __caller_saved_ft9_offset)(\base)
	\op     ft10, (\offset + __caller_saved_ft10_offset)(\base)
	\op     ft11, (\offset + __caller_saved_ft11_offset)(\base)
.endm

.macro __do_callee_saved_f op, base, offset
	\op     fs0, (\offset + __callee_saved_fs0_offset)(\base)
	\op     fs1, (\offset + __callee_saved_fs1_offset)(\base)
	\op     fs2, (\offset + __callee_saved_fs2_offset)(\base)
	\op     fs3, (\offset + __callee_saved_fs3_offset)(\base)
	\op     fs4, (\offset + __callee_saved_fs4_offset)(\base)
	\op     fs5, (\offset + __callee_saved_fs5_offset)(\base)
	\op     fs6, (\offset + __callee_saved_fs6_offset)(\base)
	\op     fs7, (\offset + __callee_saved_fs7_offset)(\base)
	\op     fs8, (\offset + __callee_saved_fs8_offset)(\base)
	\op     fs9, (\offset + __callee_saved_fs9_offset)(\base)
	\op     fs10, (\offset + __callee_saved_fs10_offset)(\base)
	\op     fs11, (\offset + __callee_saved_fs11_offset)(\base)
.endm

/**
 * Store/load X/F registers into memory
 *
 * Params
 * : base - register containing the base address
 * : offset - immediate offset added to the base address
 */

.macro store_caller_saved_x base, offset
	__do_caller_saved_x REG_S, \base, \offset
.endm

.macro load_caller_saved_x base, offset
	__do_caller_saved_x REG_L, \base, \offset
.endm

.macro store_callee_saved_x base, offset
	__do_callee_saved_x REG_S, \base, \offset
.endm

.macro load_callee_saved_x base, offset
	__do_callee_saved_x REG_L, \base, \offset
.endm

.macro store_caller_saved_f base, offset
	__do_caller_saved_f REG_FS, \base, \offset
.endm

.macro load_caller_saved_f base, offset
	__do_caller_saved_f REG_FL, \base, \offset
.endm

.macro store_callee_saved_f base, offset
	__do_callee_saved_f REG_FS, \base, \offset
.endm

.macro load_callee_saved_f base, offset
	__do_callee_saved_f REG_FL, \base, \offset
.endm
