/**
 * @file
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_ASM_H_
#define MIPS_ASM_H_

#ifdef __ASSEMBLER__

#if (_MIPS_SZPTR == 32)

#define PTR_ADD   add
#define PTR_ADDU  addu
#define PTR_ADDI  addi
#define PTR_ADDIU addiu
#define PTR_SUB   sub
#define PTR_SUBU  subu
#define PTR_L     lw
#define PTR_S     sw
#define PTR_LA    la
#define PTR_LI    li
#define PTR_SLL   sll
#define PTR_SLLV  sllv
#define PTR_SRL   srl
#define PTR_SRLV  srlv
#define PTR_SRA   sra
#define PTR_SRAV  srav

#elif (_MIPS_SZPTR == 64)

#define PTR_ADD   dadd
#define PTR_ADDU  daddu
#define PTR_ADDI  daddi
#define PTR_ADDIU daddiu
#define PTR_SUB   dsub
#define PTR_SUBU  dsubu
#define PTR_L     ld
#define PTR_S     sd
#define PTR_LA    dla
#define PTR_LI    dli
#define PTR_SLL   dsll
#define PTR_SLLV  dsllv
#define PTR_SRL   dsrl
#define PTR_SRLV  dsrlv
#define PTR_SRA   dsra
#define PTR_SRAV  dsrav

#endif /* _MIPS_SZPTR */

#if (_MIPS_SZLONG == 32)

#define LONG_ADD   add
#define LONG_ADDU  addu
#define LONG_ADDI  addi
#define LONG_ADDIU addiu
#define LONG_SUB   sub
#define LONG_SUBU  subu
#define LONG_L     lw
#define LONG_LL    ll
#define LONG_SC    sc
#define LONG_S     sw
#define LONG_SP    swp
#define LONG_SLL   sll
#define LONG_SLLV  sllv
#define LONG_SRL   srl
#define LONG_SRLV  srlv
#define LONG_SRA   sra
#define LONG_SRAV  srav
#define LONG_INS   ins
#define LONG_EXT   ext

#elif (_MIPS_SZLONG == 64)

#define LONG_ADD   dadd
#define LONG_ADDU  daddu
#define LONG_ADDI  daddi
#define LONG_ADDIU daddiu
#define LONG_SUB   dsub
#define LONG_SUBU  dsubu
#define LONG_L     ld
#define LONG_LL    lld
#define LONG_SC    scd
#define LONG_S     sd
#define LONG_SP    sdp
#define LONG_SLL   dsll
#define LONG_SLLV  dsllv
#define LONG_SRL   dsrl
#define LONG_SRLV  dsrlv
#define LONG_SRA   dsra
#define LONG_SRAV  dsrav
#define LONG_INS   dins
#define LONG_EXT   dext

#endif /* _MIPS_SZLONG */

/**
 * ENTRY - declare routine entry point
 */
#define ENTRY(symbol)        \
	.globl symbol;           \
	.type symbol, @function; \
	.ent symbol, 0;          \
symbol:                      \
	.cfi_startproc;          \
	.insn

/**
 * LEAF - declare leaf routine
 */
#define LEAF(symbol)         \
	.globl symbol;           \
	.align 2;                \
	.type symbol, @function; \
	.ent symbol, 0;          \
symbol:                      \
	.frame $sp, 0, $ra;      \
	.cfi_startproc;          \
	.insn

/**
 * NESTED - declare nested routine entry point
 */
#define NESTED(symbol, framesize, rpc) \
	.globl symbol;                     \
	.align 2;                          \
	.type symbol, @function;           \
	.ent symbol, 0;                    \
symbol:                                \
	.frame $sp, framesize, rpc;        \
	.cfi_startproc;                    \
	.insn

/**
 * END - mark end of function
 */
#define END(function) \
	.cfi_endproc;     \
	.end function;    \
	.size function, .- function

#endif /* __ASSEMBLER__ */

#endif /* MIPS_ASM_H_ */
