/**
 * @file
 * @brief
 *
 * @date 17.10.23
 * @author Aleksey Zhmulin
 */
#include <hal/cpu.h>
#include <hal/reg.h>

.weak gicv3_init_el3

/* The boot x0, in .data because .bss is zeroed much later */
.section .data
.global aarch64_boot_x0
.align 3
aarch64_boot_x0:
	.quad 0

.section .bootcode, "ax", %progbits
.global aarch64_reset_handler

aarch64_reset_handler:
	/* Save the device tree address the firmware left in x0; the first
	 * writer wins, so a secondary cannot displace the primary's value */
	adrp    x1, aarch64_boot_x0
	add     x1, x1, #:lo12:aarch64_boot_x0
	ldr     x2, [x1]
	cbnz    x2, 1f
	str     x0, [x1]
1:

	/* x20: 0 primary, 1 secondary. x21: the logical CPU id. Both are
	 * callee-saved and stay live down to el1_entry. */
	mov     x20, xzr
	mov     x21, xzr
	b       aarch64_common_entry

#ifdef SMP
.global aarch64_ap_entry
aarch64_ap_entry:
	/* PSCI CPU_ON enters here, x0 = the context id, i.e. the logical id */
	mov     x21, x0
	mov     x20, #1
	b       aarch64_common_entry

.global aarch64_ap_entry_mpidr
aarch64_ap_entry_mpidr:
	/* A spin table enters here with x0..x3 zeroed, so the logical id is
	 * read out of MPIDR: Aff0, as ap_mpidr() in smp.c hands it out */
	mrs     x21, mpidr_el1
	and     x21, x21, #0xff
	mov     x20, #1
#endif /* SMP */

aarch64_common_entry:
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
#ifdef SMP
	cbz     x20, 8f
	/* A secondary takes the sp the primary left at aarch64_ap_boot[x21],
	 * whose entries are 64 bytes each (see smp.c) */
	ldr     x0, =aarch64_ap_boot
	add     x0, x0, x21, lsl #6
	ldr     x0, [x0]
	b       9f
8:
#endif
	ldr     x0, =_stack_top
#ifdef SMP
9:
#endif
	mov     sp, x0

	/* Disable the MMU for EL3 */
	dsb     sy
	mrs     x0, sctlr_el3
	bl      flush_cache_if_mmu_on
	msr     sctlr_el3, xzr
	isb

	/* Reset SCTLR_EL2 and HCR_EL2 */
	msr     sctlr_el2, xzr
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
#ifdef SMP
	cbz     x20, 8f
	/* A secondary takes the sp the primary left at aarch64_ap_boot[x21],
	 * whose entries are 64 bytes each (see smp.c) */
	ldr     x0, =aarch64_ap_boot
	add     x0, x0, x21, lsl #6
	ldr     x0, [x0]
	b       9f
8:
#endif
	ldr     x0, =_stack_top
#ifdef SMP
9:
#endif
	mov     sp, x0

	/* Disable the MMU for EL2 */
	dsb     sy
	mrs     x0, sctlr_el2
	bl      flush_cache_if_mmu_on
	msr     sctlr_el2, xzr
	isb

	/* EL1 stage 2 address translation disabled */
	/* EL1 Execution state is AArch64 */
	mov     x0, #(HCR_EL2_RW)
	msr     hcr_el2, x0

	/* Reset SCTLR_EL1 */
	msr     sctlr_el1, xzr

	/* Don't trap to EL2 from EL1 */
	msr     hstr_el2, xzr
	msr     cptr_el2, xzr

	/* Reset VTTBR_EL2 because TLB invalidation depends on the VMID */
	msr     vttbr_el2, xzr

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
#ifdef SMP
	cbz     x20, 8f
	/* A secondary takes the sp the primary left at aarch64_ap_boot[x21],
	 * whose entries are 64 bytes each (see smp.c) */
	ldr     x0, =aarch64_ap_boot
	add     x0, x0, x21, lsl #6
	ldr     x0, [x0]
	b       9f
8:
#endif
	ldr     x0, =_stack_top
#ifdef SMP
9:
#endif
	mov     sp, x0

	/* Disable the MMU for EL0 and EL1 */
	dsb     sy
	mrs     x0, sctlr_el1
	bl      flush_cache_if_mmu_on
	msr     sctlr_el1, xzr
	isb

	/* Set up exception vectors */
	adr     x0, aarch64_kernel_vectors
	msr     vbar_el1, x0

	/* The logical CPU id, which cpu_get_id() reads and every cpudata
	 * variable is indexed by. Its reset value is UNKNOWN. */
	msr     tpidr_el1, x21

	/* Unmask System Errors */
	msr     daifclr, #(DAIF_A_imm)

	/* Enable FP/SIMD */
	mov     x0, #(0x3 << 20)
	msr     cpacr_el1, x0

	/* A secondary skips what is done once per image, not once per core */
#ifdef SMP
	cbz     x20, 7f
	mov     x0, x21
	bl      aarch64_ap_start
	b       die
7:
#endif

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

	/* Don't copy if same address */
	cmp     x0, x1
	beq     data_loop_done

data_loop:
	ldr     x3, [x1, 0]
	str     x3, [x0, 0]
	add     x0, x0, #8
	add     x1, x1, #8
	subs    x2, x2, #8
	bgt     data_loop
data_loop_done:

#if NCPU > 1
	/* Copy the initialised per-CPU block over the other NCPU - 1 copies */
	bl      cpudata_init
#endif /* NCPU > 1 */

	/* Jump to arch-independent part */
	bl      kernel_start

	/* Returning from kernel_start */
die:
	b       die

flush_cache_if_mmu_on:
	/* Check if the bootloader left the MMU or the data cache on; if so, the
	 * image and everything else it left behind may be sitting in a cache the
	 * caller is about to turn off. SCTLR is held in x22, since x20 and x21
	 * are live. */
	mov     x19, lr
	mov     x22, x0
	/* Two tests: M and C are not a single logical immediate */
	tst     x22, SCTLR_ELn_M
	bne     1f
	tst     x22, SCTLR_ELn_C
	beq     0f
1:

	/* First clean what we need to keep, by virtual address. */
	ldr     x0, =_data_lma
	ldr     x1, =_data_len
	bl      dcache_flush

	ldr     x0, =_rodata_lma
	ldr     x1, =_rodata_len
	bl      dcache_flush

	ldr     x0, =_text_lma
	ldr     x1, =_text_len
	bl      dcache_flush

	/* Then drop every other line, while SCTLR.C is still set: the page
	 * tables about to be built must not find the bootloader's leftovers */
	bl      dcache_inval_all_sw
	ic      iallu
	dsb     sy
	isb
0:
	ret     x19

/* Invalidate the whole data cache by set/way, which reaches lines whose
 * addresses are not known, using registers only. Transcribed from U-Boot
 * __asm_invalidate_dcache_all, including its FEAT_CCIDX layout of
 * CCSIDR_EL1. */
dcache_inval_all_sw:
	dsb     sy
	mrs     x10, clidr_el1
	ubfx    x11, x10, #24, #3       /* LoC */
	cbz     x11, 9f
	mov     x15, lr
	mrs     x16, id_aa64mmfr2_el1
	ubfx    x16, x16, #20, #4       /* FEAT_CCIDX */
	mov     x0, #0
2:
	add     x12, x0, x0, lsl #1
	lsr     x12, x10, x12
	and     x12, x12, #7            /* cache type at this level */
	cmp     x12, #2
	b.lt    8f                      /* no data or unified cache here */
	lsl     x12, x0, #1
	msr     csselr_el1, x12
	isb
	mrs     x6, ccsidr_el1
	ubfx    x2, x6, #0, #3          /* log2(line length) - 4 */
	cbz     x16, 3f
	ubfx    x3, x6, #3, #21         /* associativity, CCIDX */
	ubfx    x4, x6, #32, #24        /* sets, CCIDX */
	b       4f
3:
	ubfx    x3, x6, #3, #10         /* associativity */
	ubfx    x4, x6, #13, #15        /* sets */
4:
	add     x2, x2, #4
	clz     w5, w3
5:
	mov     x6, x3
6:
	lsl     x7, x6, x5
	orr     x9, x12, x7
	lsl     x7, x4, x2
	orr     x9, x9, x7
	dc      isw, x9
	subs    x6, x6, #1
	b.ge    6b
	subs    x4, x4, #1
	b.ge    5b
8:
	add     x0, x0, #1
	cmp     x11, x0
	b.gt    2b
	mov     x0, #0
	msr     csselr_el1, x0
	dsb     sy
	isb
	mov     lr, x15
9:
	ret
