/**
 * @file
 * @brief Pseudo-driver for the loopback interface.
 *
 * @date 20.10.11
 * @author Anton Kozlov
 */

#include <pnet/types.h>
#include <pnet/core.h>
#include <kernel/timer.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(loopback_init);

#define QLEN 0x10

static net_packet_t packq[QLEN];
static int qlen = 0;
static int qbeg = 0;
static int qend = 0;

static net_dev_t dev = NULL;

net_dev_t net_loopback_dev_get(void) {
	return dev;
}

static int loopback_tx(net_packet_t pack, net_dev_t dev) {
	net_packet_t new_pack = net_pack_alloc(pack->node, NET_PACKET_RX, pack->data, pack->len);

	net_pack_free(pack);

	packq[qend] = new_pack;
	qend = (qend + 1) % QLEN;
	qlen ++;

	return 0;
}

static int rx(net_packet_t pack) {
	pack->dir = NET_PACKET_RX;
	return net_core_receive(dev->node, pack->data, pack->len);
}

static struct net_dev_ops loopback_ops = {
	.tx = loopback_tx,
} ;

struct sys_timer loopback_timer;

static void lb_timer_handler(struct sys_timer *timer, void *param) {
	while (qlen) {
		rx(packq[qbeg]);
		qbeg = (qbeg + 1) % QLEN;
		qlen--;
	}
}


static int loopback_init(void) {
	const int delay = 100;
	dev = net_dev_register(&loopback_ops);
	return timer_init(&loopback_timer, delay,
			lb_timer_handler, (void *) NULL);
}


