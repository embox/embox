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

static int ipl_num;

#include <prom/prom_printf.h>
static void ipl_highest(int signal) {

	prom_printf("recv high\n");
}

static int irq_queue(void) {
	struct emvisor_msghdr msg;
	int ret;

	if (0 >= (ret = emvisor_recvmsg(UV_PRDDOWNSTRM, &msg))) {
		return ret;
	}

	if (msg.type <= EMVISOR_IRQ) {
		return -ENOENT;
	}

	irq_entry(msg.type - EMVISOR_IRQ);

	return ret;
}



static void ipl_lowest(int signal) {
	ipl_disable();

	if (signal != UV_IRQ) {
		return;
	}

	while (0 < irq_queue()) {

	}

	ipl_enable();

	emvisor_send(UV_PWRUPSTRM, EMVISOR_EOF_IRQ, NULL, 0);

}

static const host_sighandler_t ipl_table[] = {
	ipl_highest,
	ipl_lowest,
};

void ipl_hnd(int signal) {
	host_signal(UV_IRQ, ipl_hnd);
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

	ipl_num = ipl;

	/*if (ipl) {*/
		/*irq_queue();*/
	/*}*/
}
