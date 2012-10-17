/**
 * @file
 *
 * @date Oct 17, 2012
 * @author: Anton Bondarev
 */

#ifndef PPC_LINKAGE_H_
#define PPC_LINKAGE_H_

#define C_ENTRY(x) \
	.text; .align 2; .globl x; .type x,@function; x:

#endif /* PPC_LINKAGE_H_ */
