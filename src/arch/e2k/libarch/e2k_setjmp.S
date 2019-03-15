/**
 * @file
 * @brief
 *
 * @date Mar 13, 2018
 * @author Anton Bondarev
 */
#include <asm/setjmp.h>
#include <e2k_api.h>

.section ".text", "ax"

/* First arg is PSP, 2nd arg is PSHTP
 * Returns new PSP value with updated PSP.ind
 */
.type update_psp_ind,@function
$update_psp_ind:
	setwd wsz = 0x4, nfx = 0x0

	/* Here and below, 12 is size of PSHTP.ind. Here we
	 * extend the sign of PSHTP.ind as stated in documentation */
	shld %dr1, (64 - 12), %dr1
	shrd %dr1, (64 - 12), %dr1
	muld %dr1, 2, %dr1

	/* Finally, PSP.ind += PSHTP.ind */
	addd %dr1, %dr0, %dr0

	E2K_ASM_RETURN

/* First arg is PCSP, 2nd arg is PCSHTP
 * Returns new PCSP value with updated PCSP.ind
 */
.type update_pcsp_ind,@function
$update_pcsp_ind:
	setwd wsz = 0x4, nfx = 0x0

	/* Here and below, 10 is size of PCSHTP.ind. Here we
	 * extend the sign of PCSHTP.ind */
	shld %dr1, (64 - 10), %dr1
	shrd %dr1, (64 - 10), %dr1

	/* Finally, PCSP.ind += PCSHTP.ind */
	addd %dr1, %dr0, %dr0

	E2K_ASM_RETURN

.global $setjmp
.type setjmp,@function
$setjmp:
	setwd wsz = 0xE, nfx = 0x0
	/* It's for db[N] registers */
	setbn rsz = 0x3, rbs = 0xA, rcur = 0x0

	/* We must disable interrupts here.
	 */
	rrd   %upsr, %dr1
	andnd %dr1, (UPSR_IE | UPSR_NMIE), %dr1
	rwd   %dr1, %upsr

	/* Store some registers to jmp_buf */
	rrd %cr0.hi, %dr1
	rrd %cr1.lo, %dr2
	rrd %cr1.hi, %dr3
	rrd %usd.lo, %dr4
	rrd %usd.hi, %dr5

	/* Prepare RF stack to flush in longjmp */
	rrd %psp.hi, %dr6
	rrd %pshtp,  %dr7
	addd 0, %dr6, %db[0]
	addd 0, %dr7, %db[1]
	disp %ctpr1, update_psp_ind
	ipd  3
	call %ctpr1, wbs = 0xA
	addd 0, %db[0], %dr6

	/* Prepare CF stack to flush in longjmp */
	rrd %pcsp.hi, %dr7
	rrd %pcshtp,  %dr8
	addd 0, %dr7, %db[0]
	addd 0, %dr8, %db[1]
	disp %ctpr1, update_pcsp_ind
	ipd  3
	call %ctpr1, wbs = 0xA
	addd 0, %db[0], %dr7

	std %dr1, [%dr0 + E2K_JMBBUFF_CR0_HI]
	std %dr2, [%dr0 + E2K_JMBBUFF_CR1_LO]
	std %dr3, [%dr0 + E2K_JMBBUFF_CR1_HI]
	std %dr4, [%dr0 + E2K_JMBBUFF_USD_LO]
	std %dr5, [%dr0 + E2K_JMBBUFF_USD_HI]
	std %dr6, [%dr0 + E2K_JMBBUFF_PSP_HI]
	std %dr7, [%dr0 + E2K_JMBBUFF_PCSP_HI]

	/* Enable interrupts */
	rrd %upsr, %dr1
	ord %dr1, (UPSR_IE | UPSR_NMIE), %dr1
	rwd %dr1, %upsr

	/* return 0 */
	adds 0, 0, %r0
	E2K_ASM_RETURN

.size $setjmp, . - $setjmp

.global $longjmp
.type longjmp,@function
$longjmp:
	setwd wsz = 0x9, nfx = 0x0

	/* We have to flush both RF and CF to memory because saved values
	 * of P[C]SHTP can be not valid here. */
	flushr
	nop 2
	flushc
	nop 3

	/* Disable interrupts */
	rrd   %upsr, %dr2
	andnd %dr2, (UPSR_IE | UPSR_NMIE), %dr2
	rwd   %dr2, %upsr

	/* Load registers previously saved in setjmp. */
	ldd [%dr0 + E2K_JMBBUFF_CR0_HI], %dr2
	ldd [%dr0 + E2K_JMBBUFF_CR1_LO], %dr3
	ldd [%dr0 + E2K_JMBBUFF_CR1_HI], %dr4
	ldd [%dr0 + E2K_JMBBUFF_USD_LO], %dr5
	ldd [%dr0 + E2K_JMBBUFF_USD_HI], %dr6
	ldd [%dr0 + E2K_JMBBUFF_PSP_HI], %dr7
	ldd [%dr0 + E2K_JMBBUFF_PCSP_HI], %dr8

	rwd %dr2, %cr0.hi
	rwd %dr3, %cr1.lo
	rwd %dr4, %cr1.hi
	rwd %dr5, %usd.lo
	rwd %dr6, %usd.hi
	rwd %dr7, %psp.hi
	rwd %dr8, %pcsp.hi

	/* Enable interrupts */
	rrd %upsr, %dr2
	ord %dr2, (UPSR_IE | UPSR_NMIE), %dr2
	rwd %dr2, %upsr

	/* Actually, we return to setjmp caller with second
	* argument of longjmp stored on r1 register. */
	adds 0, %r1, %r0
	E2K_ASM_RETURN

.size $longjmp, . - $longjmp
