/**
 * @file
 * @brief
 *
 * @date 17.10.23
 * @author Aleksey Zhmulin
 */
#include <hal/reg.h>

.weak gicv3_init_el3

.section .text
.global aarch64_reset_handler

aarch64_reset_handler:
	/* Mask Interrupts, System Errors and Debug */
	msr     daifset, #(DAIF_D_imm | DAIF_A_imm | DAIF_I_imm | DAIF_F_imm)

	/* Get current exception level */
	mrs     x0, CurrentEL
	cmp     x0, #0b1000 /* EL, bits [3:2] */
	bgt     el3_entry
	beq     el2_entry
	blo     el1_entry
	b       die

el3_entry:
	/* Setup stack */
	ldr     x0, =_stack_top
	mov     sp, x0

	/* Disable the MMU for EL3 */
	dsb     sy
	mrs     x0, sctlr_el3
	bl      flush_cache_if_mmu_on
	msr     sctlr_el3, xzr
	isb

	/* Initialize SCTLR_EL2 and HCR_EL2 */
	ldr     x0, =(SCTLR_EL2_RES1)
	msr     sctlr_el2, x0
	msr     hcr_el2, xzr

	/* EL2 Execution state is AArch64, EL0 and EL1 are in Non-Secure state */
	mov     x0, #(SCR_EL3_RW | SCR_EL3_NS)
	msr     scr_el3, x0

	/* Exception taken from AArch64 EL2h */
	mov     x0, #(SPSR_ELn_D | SPSR_ELn_A | SPSR_ELn_I | SPSR_ELn_F | SPSR_ELn_M_EL2h)
	msr     spsr_el3, x0

	/* Configure GICv3 */
	ldr     x0, =gicv3_init_el3
	cmp     x0, #0
	beq     0f
	blr     x0
0:
	/* Determine EL2 entry */
	adr     x0, el2_entry
	msr     elr_el3, x0
	isb

	/* Drop to EL2 */
	eret

el2_entry:
	/* Setup stack */
	ldr     x0, =_stack_top
	mov     sp, x0

	/* Disable the MMU for EL2 */
	dsb     sy
	mrs     x0, sctlr_el2
	bl      flush_cache_if_mmu_on
	msr     sctlr_el2, xzr
	isb

	/* EL1 Execution state is AArch64 */
	mov     x0, #(HCR_EL2_RW)
	msr     hcr_el2, x0

	/* Initialize SCTLR_EL1 */
	ldr     x0, =(SCTLR_EL1_RES1)
	msr     sctlr_el1, x0

	/* Don't trap to EL2 for CP15 traps */
	msr     hstr_el2, xzr

	/* Exception taken from AArch64 EL1h */
	mov     x0, #(SPSR_ELn_D | SPSR_ELn_A | SPSR_ELn_I | SPSR_ELn_F | SPSR_ELn_M_EL1h)
	msr     spsr_el2, x0

	/* Enable access to the physical timers at EL1 */
	mrs     x0, cnthctl_el2
	orr     x0, x0, #(CNTHCTL_EL2_EL1PCTEN | CNTHCTL_EL2_EL1PCEN)
	msr     cnthctl_el2, x0

	/* Set the address to return to our return address */
	adr     x0, el1_entry
	msr     elr_el2, x0
	isb

	/* Drop to EL1 */
	eret

el1_entry:
	/* Setup stack */
	ldr     x0, =_stack_top
	mov     sp, x0

	/* Disable the MMU for EL0 and EL1 */
	dsb     sy
	mrs     x0, sctlr_el1
	bl      flush_cache_if_mmu_on
	msr     sctlr_el1, xzr
	isb

	/* Set up exception vectors */
	adr     x0, aarch64_trap_table
	msr     vbar_el1, x0

	/* Unmask System Errors */
	msr     daifclr, #(DAIF_A_imm)

	/* Enable FP/SIMD */
	mov     x0, #(0x3 << 20)
	msr     cpacr_el1, x0

	/* Zero .bss */
	ldr     x0, =_bss_vma
	mov     x1, #0
	ldr     x2, =_bss_len
bss_loop:
	str     x1, [x0, 0]
	add     x0, x0, #8
	subs    x2, x2, #8
	b.gt    bss_loop

	/* Copy .data */
	ldr     x0, =_data_vma
	ldr     x1, =_data_lma
	ldr     x2, =_data_len
data_loop:
	ldr     x3, [x1, 0]
	str     x3, [x0, 0]
	add     x0, x0, #8
	add     x1, x1, #8
	subs    x2, x2, #8
	b.gt    data_loop

	/* Jump to arch-independent part */
	bl      kernel_start

	/* Returning from kernel_start */
die:
	b       die

flush_cache_if_mmu_on:
	/* Check if bootloader left MMU on; if so, it's neccessary to flush cache
	 * to make sure all the data is actually in RAM, not in some buffers */
	mov     x19, lr
	ands    x0, x0, SCTLR_ELn_M
	beq     0f

	ldr     x0, =_data_lma
	ldr     x1, =_data_len
	bl      dcache_flush

	ldr     x0, =_rodata_lma
	ldr     x1, =_rodata_len
	bl      dcache_flush

	ldr     x0, =_text_lma
	ldr     x1, =_text_len
	bl      dcache_flush
0:
	ret     x19
