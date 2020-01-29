/**
 * @file
 *
 * @brief
 *
 * @date 28.01.2020
 * @author Nastya Nizharadze
 */
#include <asm/entry.h>

.align 4
.global ptregs_jmp
ptregs_jmp:
	mv      sp, a0
	RESTORE_ALL
	ret
