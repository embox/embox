/**
 * @file
 * @brief
 *
 * @author Anton Kozlov
 * @date 06.10.2014
 */
#include <asm/regs.h>
#include <asm/cp15.h>
#include <hal/cpu.h>

#include <framework/mod/options.h>

#define MULTIPROCESSING OPTION_GET(NUMBER, multi_processing)

.section .text

.global hardware_init_hook
.global setup_static_excpt_table

hardware_init_hook:
#if MULTIPROCESSING
	/* check primary core */
	mrc     p15, 0, r0, c0, c0, 5
	ubfx    r0, r0, #0, #2
	cmp     r0, #0
	bne     smp_halt
#endif

// #ifdef CORTEX_A9
// 	ldr     r0, =_monitor_vectors
// 	mcr     p15, 0, r0, c12, c0, 1
// #endif

	/* setup c1, Control Register */
	mrc     p15, 0, r0, c1, c0, 0
	orr     r0, r0, #CR_A /* enabling data abort on unaligned mem access */
	orr     r0, r0, #CR_C /* enabling data caching */
	orr     r0, r0, #CR_Z /* enabling program flow prediction */
	orr     r0, r0, #CR_I /* enabling instruction caching */
	mcr     p15, 0, r0, c1, c0, 0

	/* setup c1, Auxiliary Control Register */
	mrc     p15, 0, r0, c1, c0, 1
	orr     r0, r0, #ACR_L2EN /* enabling L2 cache */
	mcr     p15, 0, r0, c1, c0, 1
	mov     pc, lr

#if MULTIPROCESSING
smp_halt:
	wfe
#ifdef SMP
	/* TODO: SMP support */
#endif
	b       smp_halt
#endif
