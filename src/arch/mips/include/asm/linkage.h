/**
 * @file
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_LINKAGE_H_
#define MIPS_LINKAGE_H_

#define C_ENTRY(name)	.globl name; .align 4; name


#define PTR_ADD		add
#define PTR_ADDU	addu
#define PTR_ADDI	addi
#define PTR_ADDIU	addiu
#define PTR_SUB		sub
#define PTR_SUBU	subu
#define PTR_L		lw
#define PTR_S		sw
#define PTR_LA		la
#define PTR_LI		li
#define PTR_SLL		sll
#define PTR_SLLV	sllv
#define PTR_SRL		srl
#define PTR_SRLV	srlv
#define PTR_SRA		sra
#define PTR_SRAV	srav


   /*
 * LEAF - declare leaf routine
 */
#define LEAF(symbol)                                    \
		.globl	symbol;                         \
		.align	2;                              \
		.type	symbol, @function;              \
		.ent	symbol, 0;                      \
symbol:		.frame	sp, 0, ra

/*
 * NESTED - declare nested routine entry point
 */
#define NESTED(symbol, framesize, rpc)                  \
		.globl	symbol;                         \
		.align	2;                              \
		.type	symbol, @function;              \
		.ent	symbol, 0;                       \
symbol:		.frame	sp, framesize, rpc


/*
 * END - mark end of function
 */
#define	END(function)                                   \
		.end	function;		        \
		.size	function, .-function


#endif /* MIPS_LINKAGE_H_ */
