/**
 * @file
 *
 * @date   08.10.2020
 * @author Alexander Kalmuk
 */

.syntax unified
.arch   armv8-m.main

.equ RESET_STAT_REG, 0x500000BC

.section text_retained
.thumb
.thumb_func
.align  2
.globl  wakeup_reset_handler
.type   wakeup_reset_handler, %function
wakeup_reset_handler:
	ldr     r1, =RESET_STAT_REG
	ldr     r2, [r1]
	cbnz    r2, .cold_reset            /* if RESET_STAT_REG != 0 then jump to .cold_reset */

	ldr     r3, =wakeup_from_deepsleep
	bx      r3
.cold_reset:
	ldr     r3, =start
	bx      r3
.size   wakeup_reset_handler, . - wakeup_reset_handler

.section .text
.thumb
.thumb_func
.align  2
.globl  set_wakeup_reset_handler
.type   set_wakeup_reset_handler, %function
set_wakeup_reset_handler:
	ldr     r0, =0x20000004
	ldr     r1, =wakeup_reset_handler
	str     r1, [r0]
	bx      lr
.size   set_wakeup_reset_handler, . - set_wakeup_reset_handler
