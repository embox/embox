/**
 * @file arasan.c
 * @brief Arasan ethernet driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 27.12.2016
 */

#include <errno.h>
#include <sys/mman.h>

#include <util/log.h>

#include <drivers/common/memory.h>
#include <hal/reg.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <mem/misc/pool.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

/* Internal I/O space mapping */
#define BASE_ADDR  OPTION_GET(NUMBER, base_addr)
#define ARASAN_IRQ OPTION_GET(NUMBER, irq_num)

static int arasan_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

static int arasan_open(struct net_device *dev) {
	return 0;
}

static int arasan_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver arasan_drv_ops = {
	.xmit = arasan_xmit,
	.start = arasan_open,
	.set_macaddr = arasan_set_macaddr
};

static irq_return_t arasan_int_handler(unsigned int irq_num, void *dev_id) {

	return 0;
}

EMBOX_UNIT_INIT(arasan_init);
static int arasan_init(void) {
        struct net_device *nic;

        if (NULL == (nic = etherdev_alloc(0))) {
                return -ENOMEM;
        }

	irq_attach(ARASAN_IRQ, arasan_int_handler, 0, nic, "arasan");

        nic->drv_ops = &arasan_drv_ops;

	return inetdev_register_dev(nic);
}

static struct periph_memory_desc arasan_mem = {
	.start = BASE_ADDR,
	.len   = 0x200
};

PERIPH_MEMORY_DEFINE(arasan_mem);
