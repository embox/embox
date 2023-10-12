/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov
 */

#include <framework/mod/options.h>

#define MMU_BOOT      OPTION_GET(BOOLEAN, mmu_boot)
#define MMU_FAST_BOOT OPTION_GET(BOOLEAN, use_fast_boot)

.weak hardware_init_hook
.weak software_init_hook
.weak kernel_start

#if MMU_BOOT
#include <asm/cp15.h>
/* Put tables to text section to avoid setting it to zero during clearing bss */
.section .text.bootcode
.align 14
boot_tables: .space 1024 * 16

.section .text.bootcode
boot_mmu_setup:
	/* Setup MMU base */
	ldr     r0, =boot_tables
	mcr     p15, 0, r0, c2, c0, 0

	/* Map whole address space 1-1 */
	mov     r1, #0
	mov     r2, #0x1000
l1:
	/* Build supersection desc in R3 */
	orr     r3, r1, # 0x6
	orr     r3, r3, # 0xC00

	/* Write supersection desc */
	str     r3, [r0]

	add     r1, r1, # 0x00100000
	add     r0, r0, #4
	subs    r2, r2, #1
	bne     l1

	/* Setup domains */
	mov     r0, #0xff
	orr     r0, r0, lsl #8
	orr     r0, r0, lsl #16
	mcr     p15, 0, r0, c3, c0, 0

	ldr     r0, =boot_tables
	mcr     p15, 0, r0, c2, c0, 0
	mrc     p15, 0, r0, c2, c0, 0
	/* Turn on MMU */
	mrc     p15, 0, r0, c1, c0, 0
	orr     r0, r0, #CR_M
	mcr     p15, 0, r0, c1, c0, 0

	mov     pc, lr

boot_mmu_finish:
	mrc     p15, 0, r0, c1, c0, 0
	and     r0, r0, #~CR_M
	mcr     p15, 0, r0, c1, c0, 0

	mov     pc, lr
#endif

.syntax unified /* To make `subs' instruction available on Thumb16 */

.global bootldr_start
.type bootldr_start, %function

.section .text.bootcode

bootldr_start:
	ldr     r0, =hardware_init_hook
	cmp     r0, #0
	beq     sw_init_hook
	bl      hardware_init_hook

sw_init_hook:

#if MMU_BOOT
	bl      boot_mmu_setup
#endif

/* zero bss */
#if MMU_FAST_BOOT
	ldr     r4, =_bss_vma
	ldr     r9, =_bss_vma
	ldr     r5, =_bss_len
	add     r9, r9, r5
	mov     r5, #0
	mov     r6, #0
	mov     r7, #0
	mov     r8, #0
	b       2f
1:
	// store multiple at r4.
	stmia   r4!, {r5-r8}
	// If we are still below _bss_vma+_bss_len, loop.
2:
	cmp     r4, r9
	blo     1b
#else
	ldr     r0, =_bss_vma
	movs    r1, #0
	ldr     r2, =_bss_len
bss_loop:
	str     r1, [r0]
	adds    r0, r0, #4
	subs    r2, r2, #4
	bne     bss_loop
#endif

/* copy data section */
	ldr     r0, =_data_vma
	ldr     r1, =_data_lma
#if MMU_FAST_BOOT
	sub     r2, r1, r0
	beq     data_loop_done ;@ Don't copy if same address
	ldr     r2, =_data_lma
	ldr     r3, =_data_len
	add     r2, r2, r3
data_loop:
	ldmia   r1!, {r4-r7}
	stmia   r0!, {r4-r7}
	cmp     r1, r2
	blo     data_loop

#else

	cmp     r0, r1
	beq     data_loop_done ;@ Don't copy if same address

	ldr     r2, =_data_len

	cmp     r2, #0
	beq     data_loop_done ;@ Don't copy if data len is zero
data_loop:
	ldr     r3, [r1]
	str     r3, [r0]
	adds    r0, r0, #4
	adds    r1, r1, #4
	subs    r2, r2, #4
	bne     data_loop
#endif
data_loop_done:

#if MMU_BOOT
	bl      boot_mmu_finish
#endif

	ldr     r0, =software_init_hook
	cmp     r0, #0
	beq     krn_start
	bl      software_init_hook

krn_start:
	/* kernel start */
	bl      kernel_start

/* returning from kernel_start */
die:
	b       die
