/**
 * @file
 * @brief
 *
 * @date 07.01.11
 * @author Anton Bondarev
 */

 #include <asm/gdt.h>

	.global gdt_flush
gdt_flush:
	movl 4(%esp), %eax
	lgdt (%eax)

	movw $(__KERNEL_DS), %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ljmp  $(__KERNEL_CS), $flush
flush:
	ret
