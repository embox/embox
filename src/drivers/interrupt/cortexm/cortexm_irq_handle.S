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

.thumb_func
.global interrupt_handle_enter
interrupt_handle_enter:
    mov    r0, sp
    push   {lr}
    # "In the Cortex-M7 processor CCR.STKALIGN is read-only and has a value of 1.
    # This means that the exception stack frame starting address
    # is always 8-byte aligned."
    # So we align it for 8 byte for all cortex-m (here, 60 + sizeof (lr))
    sub    sp, #60
    str    r0, [sp, #56]
    stmia  sp, {r0 - r12, lr}

    # Pass saved irq_enter_ctx
    mov    r0, sp

    # Pass struct irq_saved_state
    sub    sp, #112
    mov    r1, sp

    bl     interrupt_handle

.thumb_func
.global __irq_trampoline
__irq_trampoline:

    mov    sp,  r0
    # Return from interrupt handling to usual mode
    # http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Babefdjc.html
    bx     r1

.thumb_func
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

.thumb_func
.global __pending_handle
__pending_handle:
    add    r1, r1, #52
    ldmdb  r1, {r2 - r12}

    # Call critical_dispatch_pending if we are already exited
    # from all nested interrupt handlers. If we were interrupted
    # from irq handler (3 bit is zero - that's why we use 0x08)
    # then just return back. If 3 bit is set to one, it means we were
    # interrupted from usual thread mode, so call critical_dispatch_pending
    # safely.
    mov    r1, #0xffffff08
    and    r0, r0, #0xffffff08
    cmp    r0, r1
    bne.n  __call_pendsv
    bl critical_dispatch_pending

__call_pendsv:
    # Generate PendSV interrupt
    mov    r0, #0x0
    msr    BASEPRI, r0
    cpsid  i
    bl     nvic_set_pendsv
    cpsie  i
    # DO NOT RETURN
1:  b      1b
