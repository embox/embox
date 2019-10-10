/**
 * @
 *
 * @date Sep 10, 2019
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <e2k_api.h>
#include <asm/ptrace.h>


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

/* TODO Save and restore Global Registers. */

C_ENTRY(vfork):
	/* sizeof pt_regs */
	getsp -(E2K_PT_REGS_SIZE), %dr0
	setwd wsz = 0x14, nfx = 0x1
	/* It's for db[N] registers */
	setbn rsz = 0x3, rbs = 0x10, rcur = 0x0

	/* We must disable interrupts here */
	disp %ctpr1, ipl_save
	ipd  3
	call %ctpr1, wbs = 0x10
	/* Store current IPL to dr9 */
	addd 0, %db[0], %dr9

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
	call %ctpr1, wbs = 0x10
	addd 0, %db[0], %dr6

	/* Prepare CF stack to flush in longjmp */
	rrd %pcsp.hi, %dr7
	rrd %pcshtp,  %dr8
	addd 0, %dr7, %db[0]
	addd 0, %dr8, %db[1]
	disp %ctpr1, update_pcsp_ind
	ipd  3
	call %ctpr1, wbs = 0x10
	addd 0, %db[0], %dr7

	/* Enable interrupts */
	addd 0, %dr9, %db[0]
	disp %ctpr1, ipl_restore
	ipd  3
	call %ctpr1, wbs = 0x10

	std %dr1, [%dr0 + E2K_PT_REGS_CR0_HI]
	std %dr2, [%dr0 + E2K_PT_REGS_CR1_LO]
	std %dr3, [%dr0 + E2K_PT_REGS_CR1_HI]
	std %dr4, [%dr0 + E2K_PT_REGS_USD_LO]
	std %dr5, [%dr0 + E2K_PT_REGS_USD_HI]
	std %dr6, [%dr0 + E2K_PT_REGS_PSP_HI]
	std %dr7, [%dr0 + E2K_PT_REGS_PCSP_HI]

	disp    %ctpr1, vfork_body
	ct      %ctpr1

C_ENTRY(ptregs_jmp):
	setwd wsz = 0x14, nfx = 0x0
	setbn rsz = 0x3, rbs = 0x10, rcur = 0x0

	/* We must disable interrupts here */
	disp %ctpr1, ipl_save
	ipd  3
	call %ctpr1, wbs = 0x10
	/* Store current IPL to dr9 */
	addd 0, %db[0], %dr9

	/* We have to flush both RF and CF to memory because saved values
	 * of P[C]SHTP can be not valid here. */
	E2K_ASM_FLUSH_CPU

	/* Load registers previously saved in setjmp. */
	ldd [%dr0 + E2K_PT_REGS_CR0_HI], %dr2
	ldd [%dr0 + E2K_PT_REGS_CR1_LO], %dr3
	ldd [%dr0 + E2K_PT_REGS_CR1_HI], %dr4
	ldd [%dr0 + E2K_PT_REGS_USD_LO], %dr5
	ldd [%dr0 + E2K_PT_REGS_USD_HI], %dr6
	ldd [%dr0 + E2K_PT_REGS_PSP_HI], %dr7
	ldd [%dr0 + E2K_PT_REGS_PCSP_HI], %dr8

	rwd %dr2, %cr0.hi
	rwd %dr3, %cr1.lo
	rwd %dr4, %cr1.hi
	rwd %dr5, %usd.lo
	rwd %dr6, %usd.hi
	rwd %dr7, %psp.hi
	rwd %dr8, %pcsp.hi

	ldd [%dr0 + E2K_PT_REGS_DR0], %dr8
	/* Enable interrupts */
	addd 0, %dr9, %db[0]
	disp %ctpr1, ipl_restore
	ipd  3
	call %ctpr1, wbs = 0x10

	adds 0, %r8, %r0
	E2K_ASM_RETURN
