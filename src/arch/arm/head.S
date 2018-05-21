/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov
 */
.weak hardware_init_hook
.weak software_init_hook
.weak kernel_start

.syntax unified /* To make `subs' instruction available on Thumb16 */

.global bootldr_start
.type bootldr_start, %function

.text
bootldr_start:
    ldr     r0, =hardware_init_hook
    cmp	    r0, #0
    beq	    sw_init_hook
    bl      hardware_init_hook
sw_init_hook:
/* zero bss */
    ldr	    r0, =_bss_vma
    mov	    r1, #0
    ldr	    r2, =_bss_len
bss_loop:
    str     r1, [r0]
    add     r0, r0, #4
    subs    r2, r2, #4
    bne     bss_loop

/* copy data section */
    ldr     r0, =_data_vma
    ldr     r1, =_data_lma
    ldr     r2, =_data_len
data_loop:
    ldr     r3, [r1]
    str     r3, [r0]
    add     r0, r0, #4
    add     r1, r1, #4
    subs    r2, r2, #4
    bne     data_loop

    ldr     r0, =software_init_hook
    cmp	    r0, #0
    beq	    krn_start
    bl      software_init_hook

krn_start:
    /* kernel start */
    bl	    kernel_start

/* returning from kernel_start */
die:
    b 	    die
