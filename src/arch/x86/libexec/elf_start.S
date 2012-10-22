/**
 * @file
 *
 * @date 22.10.2012
 * @author Anton Bulychev
 */

	.text

	.global elf_start
elf_start:
	mov		4(%esp), %eax

	pushl   $(0)
	pushl   $(0)
	pushl   $(0)
	pushl   $(0)
	pushl   $(0)

	mov     %esp, %ebx

	pushl   $(0)

	pushl   %ebx     // Push envp
	pushl   %ebx     // Push argv
	pushl   $(0)     // Push argc

	jmp     %eax
