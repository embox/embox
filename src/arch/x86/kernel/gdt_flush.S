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

	.global tss_flush
tss_flush:
	movw $0x2B, %ax      // Load the index of our TSS structure - The index is
		                 // 0x28, as it is the 5th selector and each is 8 bytes
                         // long, but we set the bottom two bits (making 0x2B)
                         // so that it has an RPL of 3, not zero.
	ltr %ax              // Load 0x2B into the task state register.
	ret
