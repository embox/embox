/**
 * @file
 * @brief Handle queue of packets waiting for ARP resolving.
 *
 * @date 13.12.11
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <embox/unit.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <kernel/printk.h>
#include <kernel/softirq_lock.h>
#include <kernel/time/ktime.h>
#include <mem/misc/pool.h>
#include <net/l3/arp.h>
#include <net/l3/arp_queue.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <string.h>
#include <sys/time.h>
#include <util/hashtable.h>
#include <net/neighbour.h>

int arp_queue_wait_resolve(struct sk_buff *skb) {
	int ret;
	in_addr_t daddr;

	assert(skb != NULL);
	assert(skb->dev != NULL);

	ret = rt_fib_route_ip(skb->nh.iph->daddr, &daddr);
	if (ret != 0) {
		return ret;
	}

	return neighbour_send_after_resolve(ETH_P_IP, &daddr,
			sizeof daddr, skb->dev, skb);
}
