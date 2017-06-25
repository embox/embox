/**
 * @file
 *
 * @date 17.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef PPC_LINKAGE_H_
#define PPC_LINKAGE_H_

#ifdef __ASSEMBLER__

	.macro ENTRY name
		.globl \name
		\name\():
	.endm

#endif /* __ASSEMBLER__ */

#endif /* PPC_LINKAGE_H_ */
