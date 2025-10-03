/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 03.09.25
 */

#ifndef ARM_ASM_ASM_H_
#define ARM_ASM_ASM_H_

#ifdef __ASSEMBLER__

#define ENTRY_ALIGN(name, alignment) \
	.global name;                    \
	.type name, % function;          \
	.align alignment;                \
name:                                \
	.cfi_startproc;

#define ENTRY(name)         \
	.global name;           \
	.type name, % function; \
name:                       \
	.cfi_startproc;

#define ENTRY_ALIAS(name)   \
	.global name;           \
	.type name, % function; \
name:

#define END(name) \
	.cfi_endproc; \
	.size name, .- name;

#endif /* __ASSEMBLER__ */

#endif /* ARM_ASM_ASM_H_ */
