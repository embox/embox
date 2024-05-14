/**
 * @file
 * @brief
 *
 * @author Anton Kozlov
 * @date 06.10.2014
 */
#include <hal/cpu.h>
#include <hal/reg.h>
#include <arm/fpu.h>

#include <framework/mod/options.h>

#define MULTIPROCESSING OPTION_GET(NUMBER, multi_processing)

.section .text

.global hardware_init_hook

hardware_init_hook:
#if MULTIPROCESSING
	/* Check primary core */
	mrc     p15, 0, r0, c0, c0, 5
	ubfx    r0, r0, #0, #2
	cmp     r0, #0
	bne     smp_halt
#endif

	/* Setup c1, Control Register */
	mrc     p15, 0, r0, c1, c0, 0
	orr     r0, r0, #SCTLR_A /* enabling data abort on unaligned mem access */
	orr     r0, r0, #SCTLR_C /* enabling data caching */
	orr     r0, r0, #SCTLR_Z /* enabling program flow prediction */
	orr     r0, r0, #SCTLR_I /* enabling instruction caching */
	mcr     p15, 0, r0, c1, c0, 0

	/* Setup c1, Auxiliary Control Register */
	mrc     p15, 0, r0, c1, c0, 1
	orr     r0, r0, #ACTLR_L2EN /* enabling L2 cache */
	mcr     p15, 0, r0, c1, c0, 1

#ifdef ARM_FPU_VFP
	/* Allow access to c10 & c11 coprocessors */
	mrc     p15, 0, r0, c1, c0, 2
	orr     r0, r0, #(0xf << 20)
	mcr     p15, 0, r0, c1, c0, 2
#endif

	/* Return to bootloader */
	mov     pc, lr

#if MULTIPROCESSING
smp_halt:
	wfe
#ifdef SMP
	/* TODO: SMP support */
#endif
	b       smp_halt
#endif
