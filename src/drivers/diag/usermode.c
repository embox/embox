/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    01.03.2013
 */

#include <stddef.h>
#include <hal/ipl.h>
#include <hal/arch.h>
#include <kernel/host.h>
#include <kernel/irq.h>
#include <drivers/diag.h>
#include <embox/unit.h>

#define INCHAR_IRQ (EMVISOR_IRQ_DIAG_IN - EMVISOR_IRQ)

EMBOX_UNIT_INIT(diag_mod_init);

static char diag_ch, diag_hasc;

static irq_return_t inchar_irq(unsigned int irq_nr, void *data) {
	ipl_t ipl = ipl_save();

	emvisor_recvnbody(UV_PRDDOWNSTRM, &diag_ch, sizeof(diag_ch));

	diag_hasc = 1;

	ipl_restore(ipl);

	return IRQ_HANDLED;
}

static int diag_mod_init(void) {

	return irq_attach(INCHAR_IRQ, inchar_irq, 0, NULL, "diag input");
}

static void um_diag_putc(const struct diag *diag, char ch) {
	ipl_t ipl = ipl_save();

	emvisor_send(UV_PWRUPSTRM, EMVISOR_DIAG_OUT, &ch, 1);

	ipl_restore(ipl);
}

static char um_diag_getc(const struct diag *diag) {
	ipl_t ipl = ipl_save();
	int len = 1;
	char ret;

	emvisor_send(UV_PWRUPSTRM, EMVISOR_DIAG_IN, &len, sizeof(int));

	while (!diag_hasc) {
		ipl_restore(ipl);
		arch_idle();
		ipl = ipl_save();
	}

	diag_hasc = 0;
	ret = diag_ch;

	ipl_restore(ipl);

	return ret;
}

DIAG_OPS_DECLARE(
		.putc = um_diag_putc,
		.getc = um_diag_getc,
);

