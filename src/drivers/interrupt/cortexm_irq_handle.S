/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    27.03.2017
 */

.text
.thumb
.syntax unified
.global interrupt_handle_enter
interrupt_handle_enter:

    mov    r0, sp
    # It should be a size of struct context, but we align it up to multiple of 8
    # due to ARMv7-M manual (see AAPCS 5.2.1.2)
    sub    sp, #64
    str    r0, [sp, #56]
    stmia  sp, {r0 - r12, lr}

    mov    r0, sp
    bl     interrupt_handle

.global __irq_trampoline
__irq_trampoline:

    cpsid  i
    # r0 contains SP stored on interrupt handler entry. So we keep some data
    # behind SP for a while, but interrupts are disabled by 'cpsid i'
    mov    sp,  r0
    # Return from interrupt handling to usual mode
    bx     r1

.global __pendsv_handle
__pendsv_handle:

    # 32 == sizeof (struct cpu_saved_ctx)
    add    sp, #32
    # Return to the place we were interrupted at,
    # i.e. before interrupt_handle_enter
    bx     r14

.global __pending_handle
__pending_handle:

    # Push initial saved context (state.ctx) on top of the stack
    add    r0, #32
    ldmdb  r0, {r4 - r11}
    push   {r4 - r11}

    add    r1, r1, #52
    ldmdb  r1, {r2 - r12}

    cpsie  i
    bl     critical_dispatch_pending
    cpsid  i
    # Generate PendSV interrupt
    bl     nvic_set_pendsv
    cpsie  i
    # DO NOT RETURN
1: b       1b
