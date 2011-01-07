/**
 * @file
 *
 * @brief
 *
 * @date 07.01.2011
 * @author Anton Bondarev
 */

	.global gdt_flush
gdt_flush:
	movl 4(%esp), %eax
	lgdt (%eax)

	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ljmp  $(0x8),$flush
flush:
	ret
