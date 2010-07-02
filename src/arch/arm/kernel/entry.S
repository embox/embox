/**
 * @file
 *
 * @date 28.03.2010
 * @author Nikolay Korotky
 */

/**
 * Exceptions table:
 * 0x00 - Reset
 * 0x04 - Undefined
 * 0x08 - SWI
 * 0x0C - Prefetch Abort
 * 0x10 - Data Abort
 * 0x14 - Reserved
 * 0x18 - IRQ
 * 0x1C - FIQ (Fast IRQ)
 */

.weak hardware_init_hook
.weak software_init_hook

.text
    .align 4

    .global start
start:

    ldr pc, ResetHandlerAddr
    ldr pc, UndefHandlerAddr
    ldr pc, SWIHandlerAddr
    ldr pc, PrefetchAbtHandlerAddr
    ldr pc, DataAbtHandlerAddr
    nop
    ldr pc, IRQHandlerAddr
    ldr pc, FIQHandlerAddr

ResetHandlerAddr:
    .word ResetHandler
UndefHandlerAddr:
    .word UndefHandler
SWIHandlerAddr:
    .word SWIHandler
PrefetchAbtHandlerAddr:
    .word PrefetchAbtHandler
DataAbtHandlerAddr:
    .word DataAbtHandler
IRQHandlerAddr:
    .word IRQHandler
FIQHandlerAddr:
    .word FIQHandler

/** Stubs */
UndefHandler:
    B UndefHandler
SWIHandler:
    B SWIHandler
PrefetchAbtHandler:
    B PrefetchAbtHandler
DataAbtHandler:
    B DataAbtHandler
IRQHandler:
    B IRQHandler
FIQHandler:
    B FIQHandler

/** Describe stacks' sizes */
.EQU IRQ_STACK_SIZE, 0x100
.EQU FIQ_STACK_SIZE, 0x100
.EQU ABT_STACK_SIZE, 0x100
.EQU UND_STACK_SIZE, 0x100
.EQU SVC_STACK_SIZE, 0x100

.EQU ARM_MODE_FIQ, 0x11
.EQU ARM_MODE_IRQ, 0x12
.EQU ARM_MODE_SVC, 0x13
.EQU ARM_MODE_ABT, 0x17
.EQU ARM_MODE_UND, 0x1B
.EQU ARM_MODE_USR, 0x10

/** Disable IRQ, FIQ */
.EQU I_BIT, 0x80
.EQU F_BIT, 0x40

/** Initialize stack */
.RAM_TOP:
    .word _stack_top
ResetHandler:
    ldr sp, .RAM_TOP

    msr CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #FIQ_STACK_SIZE

    msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #IRQ_STACK_SIZE

    msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #SVC_STACK_SIZE

    msr CPSR_c, #ARM_MODE_ABT | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #ABT_STACK_SIZE

    msr CPSR_c, #ARM_MODE_UND | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #UND_STACK_SIZE

    msr CPSR_c, #ARM_MODE_USR

/** Initialize memory */
    mov     r0, #0
    ldr     r1, =_bstart
    ldr     r2, =_bend
LoopZI:
    cmp     r1, r2
    strlo   r0, [r1], #4
    blo     LoopZI

    ldr     r1, =_data_lma
    ldr     r2, =_data_vma
    ldr     r3, =_data_end
LoopCpData:
    cmp     r2, r3
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo     LoopCpData

    ldr     r1, =_text_lma
    ldr     r2, =_text_vma
    ldr     r3, =_text_end
LoopCpText:
    cmp     r2, r3
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo     LoopCpText

    ldr     r1, =_reloc_lma
    ldr     r2, =_reloc_vma
    ldr     r3, =_reloc_text_end
LoopCpReloc:
    cmp     r2, r3
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo     LoopCpReloc

    //ldr		r0, =hardware_init_hook
    //bl		=hardware_init_hook

    //ldr		r0, =software_init_hook
    //bl
/** Main */
    ldr     pc, =main
/** bx      r0 */
