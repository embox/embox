/**
 * @file
 * @brief Context switch for E2K
 *
 * @date 18.03.2019
 * @author Alexander Kalmuk
 */

#include <hal/context.h>
#include <asm/linkage.h>
#include <e2k_api.h>

.section ".text", "ax"

/* TODO Save and restore Global Registers. */

C_ENTRY(context_switch):
	setwd wsz = 0x10, nfx = 0x0

	/* Save prev UPSR */
	rrd %upsr, %dr2
	std %dr2, [%dr0 + E2K_CTX_UPSR]

	/* Disable interrupts before saving/restoring context */
	rrd   %upsr, %dr2
	andnd %dr2, (UPSR_IE | UPSR_NMIE), %dr2
	rwd   %dr2, %upsr

	E2K_ASM_FLUSH_CPU

	/* Save prev CRs */
	rrd %cr0.lo, %dr2
	rrd %cr0.hi, %dr3
	rrd %cr1.lo, %dr4
	rrd %cr1.hi, %dr5
	std %dr2, [%dr0 + E2K_CTX_CR0_LO]
	std %dr3, [%dr0 + E2K_CTX_CR0_HI]
	std %dr4, [%dr0 + E2K_CTX_CR1_LO]
	std %dr5, [%dr0 + E2K_CTX_CR1_HI]

	/* Save prev stacks */
	rrd %usd.lo,  %dr3
	rrd %usd.hi,  %dr4
	rrd %psp.lo,  %dr5
	rrd %psp.hi,  %dr6
	rrd %pcsp.lo, %dr7
	rrd %pcsp.hi, %dr8
	std %dr3, [%dr0 + E2K_CTX_USD_LO]
	std %dr4, [%dr0 + E2K_CTX_USD_HI]
	std %dr5, [%dr0 + E2K_CTX_PSP_LO]
	std %dr6, [%dr0 + E2K_CTX_PSP_HI]
	std %dr7, [%dr0 + E2K_CTX_PCSP_LO]
	std %dr8, [%dr0 + E2K_CTX_PCSP_HI]

	/* Load next CRs */
	ldd [%dr1 + E2K_CTX_CR0_LO], %dr2
	ldd [%dr1 + E2K_CTX_CR0_HI], %dr3
	ldd [%dr1 + E2K_CTX_CR1_LO], %dr4
	ldd [%dr1 + E2K_CTX_CR1_HI], %dr5
	rwd %dr2, %cr0.lo
	rwd %dr3, %cr0.hi
	rwd %dr4, %cr1.lo
	rwd %dr5, %cr1.hi

	/* Load next stacks */
	ldd [%dr1 + E2K_CTX_USD_LO],  %dr3
	ldd [%dr1 + E2K_CTX_USD_HI],  %dr4
	ldd [%dr1 + E2K_CTX_PSP_LO],  %dr5
	ldd [%dr1 + E2K_CTX_PSP_HI],  %dr6
	ldd [%dr1 + E2K_CTX_PCSP_LO], %dr7
	ldd [%dr1 + E2K_CTX_PCSP_HI], %dr8
	rwd %dr3, %usd.lo
	rwd %dr4, %usd.hi
	rwd %dr5, %psp.lo
	rwd %dr6, %psp.hi
	rwd %dr7, %pcsp.lo
	rwd %dr8, %pcsp.hi

	/* Restore next UPSR */
	ldd [%dr1 + E2K_CTX_UPSR],    %dr2
	rwd %dr2, %upsr

	E2K_ASM_RETURN
