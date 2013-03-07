/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#include <assert.h>
#include <prom/prom_printf.h>
#include <kernel/host.h>
#include <hal/ipl.h>

static int ipl_num;

static void ipl_highest(int signal) {

	prom_printf("Received in highest!\n");
}

#define IRQ_BUFLEN 16
static void ipl_lowest(int signal) {
	struct emvisor_msghdr msg;
	char buf[IRQ_BUFLEN];

	if (signal != HOST_IRQ) {
		return;
	}

	assert(0 <= emvisor_recv(embox_getdownstream(), &msg,
				buf, IRQ_BUFLEN));

	assert(EMVISOR_IRQ < msg.type);

	prom_printf("Received %d\n", msg.type);

}


static const host_sighandler_t ipl_table[] = {
	ipl_highest,
	ipl_lowest,
};

void ipl_init(void) {
	host_signal(HOST_IRQ, ipl_table[ipl_num = 1]);
}

ipl_t ipl_save(void) {
	ipl_t ret = ipl_num;

	host_signal(HOST_IRQ, ipl_table[ipl_num = 0]);

	return ret;

}

void ipl_restore(ipl_t ipl) {
	host_signal(HOST_IRQ, ipl_table[ipl_num = ipl]);

	/*TODO: flush downstream*/
}
