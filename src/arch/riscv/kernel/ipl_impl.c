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
	csr = read_csr(STATUS_REG);
	write_csr(STATUS_REG, csr & ~(STATUS(IE)));
	return csr;
}

void ipl_restore(ipl_t ipl) {
	write_csr(STATUS_REG, ipl);
}
