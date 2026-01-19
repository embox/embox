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

.global vfork

.align 2
vfork:
	SAVE_ALL
	mv      a0, sp
	call    vfork_body
