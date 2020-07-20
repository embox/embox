/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    15.07.2020
 */

#include <framework/mod/options.h>

#define LTHREAD_STACK_SIZE OPTION_GET(NUMBER, stack_size)

.text
.thumb
.syntax unified

.thumb_func
.global lthread_process
lthread_process:
	push {r4, r5, lr}
	mov r4, sp
	ldr r5, =lthread_stack_top
	mov sp, r5
	bl lthread_do_process
	mov sp, r4
	pop {r4, r5, pc}

.data
.align 3
lthread_stack_start:
	.rept   LTHREAD_STACK_SIZE
	.byte   0x55
	.endr
lthread_stack_top:
