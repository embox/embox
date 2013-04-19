/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#include <assert.h>
#include <stddef.h>
#include <errno.h>
#include <kernel/host.h>
#include <kernel/umirq.h>
#include <hal/ipl.h>

#include <prom/prom_printf.h>

int ipl_num;
static int ipl_pending;

static void ipl_highest(int signal) {
	ipl_pending ++;
}

static void ipl_lowest(int signal) {

	ipl_disable();

	irq_entry();

	ipl_enable();
}

static const host_sighandler_t ipl_table[] = {
	ipl_highest,
	ipl_lowest,
};

void ipl_hnd(int signal) {
	if (signal != UV_IRQ) {
		return;
	}

	ipl_table[ipl_num](signal);
}

void ipl_init(void) {
	ipl_restore(1);
}

ipl_t ipl_save(void) {
	ipl_t ret = ipl_num;

	ipl_restore(0);

	return ret;

}

void ipl_restore(ipl_t ipl) {

	ipl_num = 0;

	if (ipl && ipl_pending) {
		ipl_pending = 0;
		ipl_lowest(UV_IRQ);
	}

	ipl_num = ipl;

}
