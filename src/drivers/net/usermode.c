/**
 * @file
 * @brief Usermode ethernet driver (supported by emvisor)
 *
 * @author  Anton Kozlov
 * @date    31.03.2013
 */

#include <errno.h>
#include <string.h>
#include <kernel/irq.h>
#include <net/etherdevice.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <arpa/inet.h>
#include <net/skbuff.h>
#include <embox/unit.h>

#include <kernel/host.h>

EMBOX_UNIT_INIT(umether_init);

struct emvisor_netstate devstate;

static int devstate_update(struct emvisor_netstate *devstate) {
	return emvisor_send(UV_PWRUPSTRM, EMVISOR_NETCTL, devstate,
			sizeof(struct emvisor_netstate));
}

static int umether_start_xmit(struct sk_buff *skb, struct net_device *dev) {
	struct emvisor_netdata_hdr dhdr = {
		.dev_id = devstate.id,
		.len = skb->len,
	};
	ipl_t ipl = ipl_save();
	int ret;

	if (devstate.id < 0) {
		ret = -ENODEV;
		goto out;
	}

	if (0 >= (ret = emvisor_sendhdr(UV_PWRUPSTRM, EMVISOR_NETDATA,
					sizeof(dhdr) + skb->len))) {
		goto out;
	}

	if (0 >= (ret = emvisor_sendn(UV_PWRUPSTRM, &dhdr, sizeof(dhdr)))) {
		goto out;
	}

	if (0 >= (ret = emvisor_sendn(UV_PWRUPSTRM, skb->mac.raw, skb->len))) {
		goto out;
	}

out:
	skb_free(skb);
	ipl_restore(ipl);
	return ret;
}

static int umether_open(struct net_device *dev) {

	if (devstate.id < 0) {
		return -ENODEV;
	}

	devstate.mode = NETSTATE_MODON;

	devstate_update(&devstate);

	return ENOERR;
}

static net_device_stats_t *umether_getstat(struct net_device *dev) {
	return &(dev->stats);
}

static int umether_stop(struct net_device *dev) {

	if (devstate.id < 0) {
		return -ENODEV;
	}

	devstate.mode = NETSTATE_MODOFF;

	devstate_update(&devstate);

	return ENOERR;
}

static int umether_setmac(struct net_device *dev, void *addr) {

	if (devstate.id < 0) {
		return -ENODEV;
	}

	memcpy(dev->dev_addr, addr, ETH_ALEN);

	memcpy(&devstate.addr, addr, ETH_ALEN);

	devstate_update(&devstate);

	return ENOERR;
}

#define SKIPBUF_LEN 64

static irq_return_t umether_irq(unsigned int irq_num, void *dev_id) {
	struct emvisor_netdata_hdr hdr;
	struct sk_buff *skb;

	emvisor_recvnbody(UV_PRDDOWNSTRM, &hdr, sizeof(hdr));

	assert(hdr.dev_id == devstate.id);

	if (!(skb = skb_alloc(hdr.len))) {
		char skipb[SKIPBUF_LEN];
		int len = hdr.len, tl;
		while (len) {
			tl = len > SKIPBUF_LEN ? SKIPBUF_LEN : len;
			emvisor_recvnbody(UV_PRDDOWNSTRM, skipb, tl);
			len -= tl;
		}

		return IRQ_HANDLED;
	}


	emvisor_recvnbody(UV_PRDDOWNSTRM, skb->mac.raw, hdr.len);

	skb->dev = (struct net_device *) dev_id;
	skb->protocol = ntohs(skb->mac.ethh->h_proto);

	netif_rx(skb);

	return IRQ_HANDLED;
}

static irq_return_t umether_irqctl(unsigned int irq_num, void *dev_id) {
	struct emvisor_netstate state;

	emvisor_recvnbody(UV_PRDDOWNSTRM, &state, sizeof(state));

	assert(devstate.id == -1 || devstate.id == state.id);

	memcpy(&devstate, &state, sizeof(devstate));

	return IRQ_HANDLED;
}

static const struct net_device_ops umether_netdev_ops = {
	.ndo_start_xmit = umether_start_xmit,
	.ndo_open = umether_open,
	.ndo_stop = umether_stop,
	.ndo_get_stats = umether_getstat,
	.ndo_set_mac_address = umether_setmac,
};

static int umether_init(void) {
	int res = 0;
	struct net_device *nic;

	nic = etherdev_alloc();
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->netdev_ops = &umether_netdev_ops;
	nic->irq = EMVISOR_IRQ_NETDATA - EMVISOR_IRQ;

	res = irq_attach(EMVISOR_IRQ_NETDATA - EMVISOR_IRQ, umether_irq,
			IF_SHARESUP, nic, "umether");
	if (res < 0) {
		return res;
	}

	res = irq_attach(EMVISOR_IRQ_NETCTL - EMVISOR_IRQ, umether_irqctl,
			IF_SHARESUP, nic, "umether");
	if (res < 0) {
		return res;
	}

	devstate.id = NETSTATE_REQUEST;

	devstate_update(&devstate);

	return inetdev_register_dev(nic);
}
