/**
 * @file
 *
 * @date 18.03.2019
 * @author Alexander Kalmuk
 */

#include <hal/ipl.h>
#include <e2k_api.h>
#include <asm/cpu_regs_types.h>

void ipl_init(void) {
	e2k_pfpfr_t pfpfr;
	e2k_fpcr_t fpcr;

	/* XXX E2k
	 * we have comment in e2k_kernel_start()
	 * XXX Disable FPU. Should be enabled later. */
	e2k_upsr_write(e2k_upsr_read() | UPSR_FE);
	e2k_upsr_write(e2k_upsr_read() | (UPSR_IE | UPSR_NMIE));

	pfpfr.word = e2k_pfpfr_read();
	pfpfr.fields.im = 1;
	pfpfr.fields.dm = 1;
	pfpfr.fields.zm = 1;
	pfpfr.fields.om = 1;
	pfpfr.fields.um = 1;
	pfpfr.fields.pm = 1;
	e2k_pfpfr_write(pfpfr.word);

	fpcr.word = e2k_fpcr_read();
	fpcr.fields.im = 1;
	fpcr.fields.dm = 1;
	fpcr.fields.zm = 1;
	fpcr.fields.om = 1;
	fpcr.fields.um = 1;
	fpcr.fields.pm = 1;
	fpcr.fields.pc = 3;
	e2k_fpcr_write(fpcr.word);
}

unsigned int ipl_save(void) {
	unsigned int upsr;
	upsr = e2k_upsr_read();
	e2k_upsr_write(upsr & ~(UPSR_IE | UPSR_NMIE));
	return upsr;
}

void ipl_restore(unsigned int ipl) {
	e2k_upsr_write(ipl);
}
