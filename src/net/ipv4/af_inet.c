/**
 * @file af_inet.c
 *
 * @brief PF_INET protocol family socket handler.
 * @date 01.12.09
 * @author Nikolay Korotky
 */

#include "kernel/module.h"
#include "lib/inet/netinet/in.h"
#include "err.h"
#include "net/protocol.h"
#include "net/udp.h"
#include "net/icmp.h"

static struct net_protocol udp_protocol = {
        .handler = udp_rcv,
};

static struct net_protocol icmp_protocol = {
        .handler = icmp_rcv,
};

static int __init inet_init(void) {
	/* Add all the base protocols. */
        if (inet_add_protocol(&icmp_protocol, IPPROTO_ICMP) < 0)
                LOG_ERROR("inet_init: Cannot add ICMP protocol\n");
        if (inet_add_protocol(&udp_protocol, IPPROTO_UDP) < 0)
    		LOG_ERROR("inet_init: Cannot add UDP protocol\n");

	/* Set the ARP module up */
	arp_init();

	/* Set the IP module up */
        ip_init();

        /* Set the ICMP layer up */
	icmp_init();

	return 0;
}
