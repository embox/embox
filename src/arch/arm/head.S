/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov
 */

#include <framework/mod/options.h>

#define MMU_BOOT OPTION_GET(BOOLEAN,mmu_boot)

.weak hardware_init_hook
.weak software_init_hook
.weak kernel_start

#if MMU_BOOT
#include <asm/cp15.h>
/* Put tables to text section to avoid setting it to zero during clearing bss */
.text
.align 14
boot_tables: .space 1024 * 16

.text
boot_mmu_setup:
	/* Setup MMU base */
	ldr r0, =boot_tables
	mcr p15, 0, r0, c2, c0, 0

	/* Map whole address space 1-1 */
	mov r1, #0
	mov r2, #0x1000
l1:
	/* Build supersection desc in R3 */
	orr r3, r1, # 0x6
	orr r3, r3, # 0xC00

	/* Write supersection desc */
	str r3, [r0]

	add r1, r1, # 0x00100000
	add r0, r0, #4
	subs r2, r2, #1
	bne l1

	/* Setup domains */
	mov r0, #0xff
	orr r0, r0, lsl #8
	orr r0, r0, lsl #16
	mcr p15, 0, r0, c3, c0, 0

	ldr r0, =boot_tables
	mcr p15, 0, r0, c2, c0, 0
	mrc p15, 0, r0, c2, c0, 0
	/* Turn on MMU */
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #CR_M
	mcr p15, 0, r0, c1, c0, 0

	mov pc, lr

boot_mmu_finish:
	mrc p15, 0, r0, c1, c0, 0
	and r0, r0, #~CR_M
	mcr p15, 0, r0, c1, c0, 0

	mov pc, lr
#endif

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

#if MMU_BOOT
	bl boot_mmu_setup
#endif

/* zero bss */
	ldr     r0, =_bss_vma
	movs    r1, #0
	ldr     r2, =_bss_len
bss_loop:
	str     r1, [r0]
	adds    r0, r0, #4
	subs    r2, r2, #4
	bne     bss_loop

/* copy data section */
	ldr     r0, =_data_vma
	ldr     r1, =_data_lma
	ldr     r2, =_data_len
data_loop:
	ldr     r3, [r1]
	str     r3, [r0]
	adds    r0, r0, #4
	adds    r1, r1, #4
	subs    r2, r2, #4
	bne     data_loop

#if MMU_BOOT
	bl boot_mmu_finish
#endif

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
