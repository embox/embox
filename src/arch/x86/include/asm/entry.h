/**
 * @file
 *
 * @brief
 *
 * @date 21.09.2011
 * @author Anton Bondarev
 */

#ifndef X86_ENTRY_H_
#define X86_ENTRY_H_

#ifdef __ASSEMBLER__
#define SAVE_ALL \
	pusha; /*pushes edi,esi,ebp,esp,ebx,edx,ecx,eax*/ \
	pushl   %ds;                                      \
	pushl   %es;                                      \
	pushl   %fs;                                      \
	pushl   %gs;


#define RESTORE_ALL \
	pop     %gs;    \
	pop     %fs;    \
	pop     %es;    \
	pop     %ds;    \
	popa;


#define SETUP_SEGMENTS \
	movl    %ss, %eax; \
	movl    %eax, %ds; \
	movl    %eax, %es;


#endif

#endif /* X86_ENTRY_H_ */
