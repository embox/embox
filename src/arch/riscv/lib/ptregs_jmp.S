/**
 * @file
 *
 * @brief
 *
 * @date 28.01.2020
 * @author Nastya Nizharadze
 */
#include <asm/asm.h>
#include <asm/entry.h>

.section .text

.global ptregs_jmp

.align 2
ptregs_jmp:
	mv      sp, a0
	RESTORE_ALL
	ret
