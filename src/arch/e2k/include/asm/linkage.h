/**
 * @file
 *
 * @date 21.03.2019
 * @author Alexander Kalmuk
 */

#ifndef ARCH_E2K_ASM_LINKAGE_H_
#define ARCH_E2K_ASM_LINKAGE_H_

#ifdef __ASSEMBLER__

#define C_ENTRY(name) \
	.global name; \
	.type name,@function; \
	name

#endif

#endif /* ARCH_E2K_ASM_LINKAGE_H_ */
