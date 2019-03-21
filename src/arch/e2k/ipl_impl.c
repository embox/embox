/**
 * @file
 *
 * @date 18.03.2019
 * @author Alexander Kalmuk
 */

#include <hal/ipl.h>
#include <e2k_api.h>

void ipl_init(void) {
	e2k_upsr_write(e2k_upsr_read() | (UPSR_IE | UPSR_NMIE));
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
