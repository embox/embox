/**
 * @file
 * @brief Context switching for x86.
 *
 * @date 26.02.2011
 * @author Eldar Abusalimov
 */

	.text
	.global context_switch
context_switch:
	movl 4(%esp), %ecx           /* Point ecx to previous registers */
	movl (%esp), %eax            /* Get return address */
	movl %eax, 0(%ecx)           /* Save it as eip */
	movl %ebx, 4(%ecx)           /* Save ebx */
	movl %edi, 8(%ecx)           /* Save edi */
	movl %esi, 12(%ecx)          /* Save esi */
	movl %ebp, 16(%ecx)          /* Save ebp */
	movl %esp, 20(%ecx)          /* Save esp */
	movl 8(%esp), %ecx           /* Point ecx to next registers */
	movl 4(%ecx), %ebx           /* Restore ebx */
	movl 8(%ecx), %edi           /* Restore edi */
	movl 12(%ecx), %esi          /* Restore esp */
	movl 16(%ecx), %ebp          /* Restore ebp */
	movl 20(%ecx), %esp          /* Restore esp */
	movl 0(%ecx), %eax           /* Get eip */
	movl %eax, (%esp)            /* Restore it as return address */
	ret
