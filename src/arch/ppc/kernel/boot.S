/**
 * @file
 *
 * @date 17.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/regs.h>

    .section .text

/**
 * Kernel start point
 */
ENTRY _start
    /**
     * Setup Machine State Register
     */
    mfmsr    r3                 /* load MSR */
    lis      r4, MSR_ILE@h      /* setup exception handling to big-endian mode */
    ori      r4, r4, MSR_ILE@l
    andc     r3, r3, r4
    lis      r4, MSR_LE@h       /* setup big-endian mode */
    ori      r4, r4, MSR_LE@l
    andc     r3, r3, r4
    lis      r4, MSR_PR@h       /* allow user- and supervisor-level instructions */
    ori      r4, r4, MSR_PR@l
    andc     r3, r3, r4
    lis      r4, MSR_IP@h       /* setup exception prefix to 0x000n_nnnn */
    ori      r4, r4, MSR_IP@l
    andc     r3, r3, r4
    mtmsr    r3                 /* set a new MSR */
    isync                       /* synchromizing */

    /**
     * Setup Interrupt Vector Prefix/Offset Registers
     */
    lis      r3, except0_hnd@h  /* setup IVPR */
    mtivpr   r3
    li       r3, except0_hnd@l  /* setup IVOR0 */
    mtivor0  r3
    li       r3, except1_hnd@l  /* setup IVOR1 */
    mtivor1  r3
    li       r3, except2_hnd@l  /* setup IVOR2 */
    mtivor2  r3
    li       r3, except3_hnd@l  /* setup IVOR3 */
    mtivor3  r3
    li       r3, except4_hnd@l  /* setup IVOR4 */
    mtivor4  r3
    li       r3, except5_hnd@l  /* setup IVOR5 */
    mtivor5  r3
    li       r3, except6_hnd@l  /* setup IVOR6 */
    mtivor6  r3
    li       r3, except7_hnd@l  /* setup IVOR7 */
    mtivor7  r3
    li       r3, except8_hnd@l  /* setup IVOR8 */
    mtivor8  r3
    li       r3, except9_hnd@l  /* setup IVOR9 */
    mtivor9  r3
    li       r3, except10_hnd@l  /* setup IVOR10 */
    mtivor10 r3
    li       r3, except11_hnd@l  /* setup IVOR11 */
    mtivor11 r3
    li       r3, except12_hnd@l  /* setup IVOR12 */
    mtivor12 r3
    li       r3, except13_hnd@l  /* setup IVOR13 */
    mtivor13 r3
    li       r3, except14_hnd@l  /* setup IVOR14 */
    mtivor14 r3
    li       r3, except15_hnd@l  /* setup IVOR15 */
    mtivor15 r3

    /**
     * Clear additional registers
     */
    li       r3, 0
    mttsr    r3                  /* clear TSR */
    mtmcsr   r3                  /* clear MCSR */
    mttbl    r3                  /* clear TBU/TBL */
    mttbu    r3
    mttbl    r3

    /**
     * Keep kernel stack
     */
    lis      r1, _stack_top@h
    ori      r1, r1, _stack_top@l

    /**
     * Clear kernel BSS
     */
    lis      r3, _bss_vma@h
    ori      r3, r3, _bss_vma@l
    lis      r4, _bss_len@h
    ori      r4, r4, _bss_len@l
    add      r4, r3, r4
    li       r0, 0
1:
    stwu     r0, 4(r3)
    cmplw    cr0, r3, r4
    blt      1b

    /**
     * Call C code
     */
    subi     r1, r1, 8           /* allocate space for sp and ra */
    b        kernel_start
