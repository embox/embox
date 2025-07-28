/**
 * @file
 *
 * @date  14.05.2019
 * @author Dmitry Kurbatov
 */

//#include <ipl_impl.h>
#include <hal/ipl.h>
#include <asm/csr.h>
#include <asm/interrupts.h>

void ipl_init(void) {
	enable_interrupts();
}

ipl_t ipl_save(void) {
	ipl_t csr;
	csr = csr_read(CSR_STATUS);
	csr_write(CSR_STATUS, csr & ~(CSR_STATUS_IE));
	return csr;
}

void ipl_restore(ipl_t ipl) {
	csr_write(CSR_STATUS, ipl);
}
