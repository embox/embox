/**
 * @file
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_LINKAGE_H_
#define MIPS_LINKAGE_H_

#include <asm/asm.h>

#ifdef __ASSEMBLER__

#define C_ENTRY(name) .globl name; .align 2; name

/*
 * LEAF - declare leaf routine
 */
#define LEAF(symbol)                       \
	.globl symbol;                         \
	.align 2;                              \
	.type  symbol, @function;              \
	.ent   symbol, 0;                      \
symbol:    .frame  $sp, 0, $ra

/*
 * NESTED - declare nested routine entry point
 */
#define NESTED(symbol, framesize, rpc)    \
	.globl symbol;                        \
	.align 2;                             \
	.type  symbol, @function;             \
	.ent   symbol, 0;                     \
symbol:    .frame $sp, framesize, rpc


/*
 * END - mark end of function
 */
#define END(function)                     \
	.end  function;                       \
	.size function, .-function

#endif /* __ASEEMBLER__ */

#endif /* MIPS_LINKAGE_H_ */
