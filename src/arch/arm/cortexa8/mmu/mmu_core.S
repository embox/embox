/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 25.11.13
 */

#include <asm/hal/mmu.h>
#include <util/binalign.h>

#define RAM_START 0x80000000
#define RAM_SIZE  0x40000000

    .section .data.translation_table

    .align 14
translation_table_start:
    .space 16384 /* default 16KB */
translation_table_end:

    .section .text

    .global mmu_init
mmu_init:
    /* setup translation table */
    ldr r0, =translation_table_start /* r0 = start of TLB */
    ldr r1, =translation_table_end   /* r1 = end of TLB */
    ldr r2, =binalign(RAM_START, 18) /* r2 = aligned start of RAM */
    ldr r3, =RAM_START + RAM_SIZE    /* r3 = end of RAM */
    mov r4, #0                       /* r4 = first region of VMEM */
    orr r4, #L1D_TYPE_SD             /* mark region as section descriptor */
    orr r4, #L1D_XN                  /* disable execution on region */
    orr r4, #L1D_AP_FULL             /* enable full access for region */
1:  cmp r4, r2                       /* do: if region >= start of RAM then */
    orrhs   r4, r4, #L1D_B | L1D_C   /*   set bufferable and cacheable flags */
    cmp r4, r3                       /* if region >= end of RAM then */
    bichs   r4, r4, #L1D_B | L1D_C   /*   clear cacheable and cacheable flags */
    str r4, [r0], #4                 /* save region into TLB, increment TLB */
    add r4, r4, #L1D_BASE(0)         /* select the next region */
    teq r0, r1                       /* while the end of TLB is reached */
    bne 1b

    /* setup c2, Translation Table Base Register 0 */
    ldr r0, =translation_table_start
    mcr p15, 0, r0, c2, c0, 0

    /* setup c3, Domain Access Control Register */
    mov r0, #0xffffffff
    mcr p15, 0, r0, c3, c0, 0

    /* exit */
    mov pc, lr
