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
.global vfork
vfork:
	SAVE_ALL
	mv      a0, sp
	j vfork_body
