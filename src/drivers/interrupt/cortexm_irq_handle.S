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

    # Pass saved irq_enter_ctx
    mov    r0, sp

    # Pass struct irq_saved_state
    sub    sp, #112
    mov    r1, sp

    bl     interrupt_handle

.global __irq_trampoline
__irq_trampoline:

    mov    sp,  r0
    # Return from interrupt handling to usual mode
    # http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Babefdjc.html
    bx     r1

.global __pendsv_handle
__pendsv_handle:

    # Here we just clear all stacked registers used by PendSV
    # 32 ==  sizeof (struct cpu_saved_ctx)
    add    sp, #32

    # Clear FPU registers if required
    tst    r14, #0x10
    bne    out
    # 72 = 18 * 4 == size of FPU part
    add    sp, #72

out:
    # Return to the place we were interrupted at,
    # i.e. before interrupt_handle_enter
    pop    {r14}
    pop    {r0}
    mov    sp, r0
    bx     r14

.global __pending_handle
__pending_handle:
    add    r1, r1, #52
    ldmdb  r1, {r2 - r12}

    bl     critical_dispatch_pending

    # Generate PendSV interrupt
    cpsid  i
    bl     nvic_set_pendsv
    cpsie  i
    # DO NOT RETURN
1:  b      1b
