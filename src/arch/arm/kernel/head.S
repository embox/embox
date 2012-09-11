/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov
 */
.weak hardware_init_hook
.weak software_init_hook
.weak kernel_start

.global bootldr_start
.type bootldr_start, %function

.text
bootldr_start:

    ldr	    r0, =_bss_vma
    mov	    r1, #0
    ldr	    r2, =_bss_len

    bl	    memset

/* copy data section */
    ldr     r0, =_data_vma
    ldr     r1, =_data_lma
    ldr     r2, =_data_len

    bl	    memcpy

hw_init_hook:
    ldr     r0, =hardware_init_hook
    cmp	    r0, #0
    beq	    sw_init_hook
    bl      hardware_init_hook

sw_init_hook:
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
