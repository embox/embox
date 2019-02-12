/**
 * @file
 *
 * @date May 9, 2018
 * @author Anton Bondarev
 */
#include <asm/modes.h>
#include <asm/regs.h>
#include <asm/cp15.h>
#include <framework/mod/options.h>

/* OPTION_GET(NUMBER,irq_stack_size) */
#define IRQ_STACK_SIZE            0x100

.text
.global start
.global reset_handler
start:
reset_handler:
    /* Make sure MMU is off */
    mrc p15, 0, r0, c1, c0, 0
    and r0, r0, #~CR_M
    mcr p15, 0, r0, c1, c0, 0

    ldr r0, =_stack_top

    msr CPSR_c, #0x16 | I_BIT | F_BIT
    mov sp, r0

    msr CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT
    mov sp, r0
    msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
    mov sp, r0
    msr CPSR_c, #ARM_MODE_ABT | I_BIT | F_BIT
    mov sp, r0
    msr CPSR_c, #ARM_MODE_UND | I_BIT | F_BIT
    mov sp, r0
    msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
    mov sp, r0

    msr CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT
    sub r0, r0, #IRQ_STACK_SIZE
    mov sp, r0

    bl setup_excpt_table

    b bootldr_start
