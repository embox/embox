/**
 * @file
 * @brief Context switching for x86.
 *
 * @date 26.02.11
 * @author Eldar Abusalimov
 */

	.text
	.global context_switch
context_switch:
	movl 0x04(%esp), %ecx          /* Point ecx to previous registers */
	movl (%esp), %eax              /* Get return address */
	movl %eax, 0x00(%ecx)          /* Save it as eip */
	movl %ebx, 0x04(%ecx)          /* Save ebx */
	movl %edi, 0x08(%ecx)          /* Save edi */
	movl %esi, 0x0c(%ecx)          /* Save esi */
	movl %ebp, 0x10(%ecx)          /* Save ebp */
	movl %esp, 0x14(%ecx)          /* Save esp */
	pushf                          /* Push flags */
	pop  0x18(%ecx)                /* ...and save them */

	movl 0x08(%esp), %ecx          /* Point ecx to next registers */
	movl 0x04(%ecx), %ebx          /* Restore ebx */
	movl 0x08(%ecx), %edi          /* Restore edi */
	movl 0x0c(%ecx), %esi          /* Restore esp */
	movl 0x10(%ecx), %ebp          /* Restore ebp */
	movl 0x14(%ecx), %esp          /* Restore esp */
	push 0x18(%ecx)                /* Push saved flags */
	popf                           /* Restore flags */
	movl 0x00(%ecx), %eax          /* Get eip */
	movl %eax, (%esp)              /* Restore it as return address */

	ret
