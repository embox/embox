#include <asm/linkage.h>

//http://dc0d32.blogspot.ru/2010/06/real-mode-in-c-with-gcc-writing.html

	.section .real_boot
	.code16
/* system entry point */
C_ENTRY(_start):
	mov     %cr0, %eax
	and     $0x0fffe, %eax
	mov     %eax, %cr0

	mov     $0x4F02, %eax
	mov     $0x11B, %ebx
	int  $0x10
