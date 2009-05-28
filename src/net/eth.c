/**
 * \file eth.c
 *
 * \date Mar 4, 2009
 * \author anton
 */
#include "misc.h"
#include "string.h"
#include "net_device.h"
#include "net_pack_manager.h"
#include "arp.h"
#include "udp.h"
#include "net.h"
#include "eth.h"

#define INTERFACES_QUANTITY 4

static unsigned char def_ip [4] = {192, 168, 0, 80};
static unsigned char def_mac [6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00};

typedef struct _CALLBACK_INFO {
	int is_busy;
	unsigned short type;
	ETH_LISTEN_CALLBACK func;
}CALLBACK_INFO;

typedef struct _IF_DEVICE {
	unsigned char ipv4_addr[4];
	net_device *net_dev;
	CALLBACK_INFO cb_info[8];
}IF_DEVICE;

static IF_DEVICE ifs[INTERFACES_QUANTITY];

void packet_dump(net_packet *);

static int rebuild_header(net_packet * pack) {
	if (NULL == pack) {
		return -1;
	}
	if (NULL == pack->sk || SOCK_RAW != pack->sk->sk_type) {
		if (NULL == arp_resolve_addr(pack, pack->nh.iph->daddr)) {
			LOG_WARN("Destanation host is unreachable\n");
			return -1;
		}
		memcpy (pack->mac.mach->src_addr, pack->netdev->hw_addr, sizeof(pack->mac.mach->src_addr));
		pack->mac.mach->type=pack->protocol;
		pack->len += sizeof(machdr);
		return 0;
	}
	return 0;
}

int eth_init() {
	char iname[6];
	int cnt = 0, i;
	TRACE("Initializing ifdevs:\n");
	for (i = 0; i < INTERFACES_QUANTITY; i ++) {
		sprintf(iname, "eth%d", i);
		if (NULL != (ifs[i].net_dev = find_net_device(iname))) {
			def_ip[3] = 80 + i;
			def_mac[5] = 1 + i;
			ifs[i].net_dev->rebuild_header = rebuild_header;
			eth_set_ipaddr(&ifs[i], def_ip);
			eth_set_macaddr(&ifs[i], def_mac);
			TRACE("Found dev %s\n", iname);
			cnt ++;
		}
	}
	return cnt;
}

void *eth_get_ifdev_by_name(const char *if_name) {
	int i;
	for (i = 0; i < INTERFACES_QUANTITY; i ++) {
		TRACE ("ifname %s, net_dev 0x%X\n", if_name, ifs[i].net_dev);
		if(0 == strncmp(if_name, ifs[i].net_dev->name, sizeof(ifs[i].net_dev->name))) {
			return &ifs[i];
		}
	}
	return NULL;
}

int eth_set_interface (char *name, char *ipaddr, char *macaddr) {
	int i;
	for (i = 0; i < INTERFACES_QUANTITY; i++ ) {
		if (0 == strcmp(name, ifs[i].net_dev->name)) {
			memcpy(ifs[i].ipv4_addr, ipaddr, sizeof(ifs[i].ipv4_addr));
			memcpy(ifs[i].net_dev->hw_addr, macaddr ,sizeof(ifs[i].net_dev->hw_addr));
			return i;
		}
	}
	return -1;
}

int eth_set_ipaddr (void *ifdev, unsigned char ipaddr[4]) {
        if (NULL == ifdev)
                return -1;
	IF_DEVICE *dev = (IF_DEVICE *)ifdev;
	memcpy(dev->ipv4_addr, ipaddr, sizeof(dev->ipv4_addr));
	return 0;
}

int eth_set_macaddr (void *ifdev, unsigned char macaddr[6]) {
	LOGGER();
        if (NULL == ifdev)
                return -1;
	IF_DEVICE *dev = (IF_DEVICE *)ifdev;
	memcpy(dev->net_dev->hw_addr, macaddr ,dev->net_dev->addr_len);
	return 0;
}
/*
int eth_set_interface (char *name, char *ipaddr, char *macaddr) {
	int i;
	for (i = 0; i < INTERFACES_QUANTITY; i++ ) {
		if (0 == strcmp(name, ifs[i].net_dev->name)) {
			memcpy(ifs[i].ipv4_addr, ipaddr, 4 * sizeof(char));
			memcpy(ifs[i].net_dev->hw_addr, macaddr ,6 * sizeof(char));
			return i;
		}
	}
	return -1;
}
*/
unsigned char *eth_get_ipaddr(void *handler)
{
	IF_DEVICE *dev = (IF_DEVICE *)handler;
	if (NULL == dev)
		return NULL;
	return dev->ipv4_addr;
}

net_device *eth_get_netdevice(void *handler)
{
	IF_DEVICE *dev = (IF_DEVICE *)handler;
	if (NULL == dev)
		return NULL;
	return dev->net_dev;
}

int eth_send (net_packet *pack)
{
	IF_DEVICE *dev = (IF_DEVICE *)pack->ifdev;

	if ((NULL == pack) || (NULL == pack->ifdev))
		return -1;

	if (ARP_PROTOCOL_TYPE != pack->protocol)
	{
		if (-1 == dev->net_dev->rebuild_header(pack))
		{
			net_packet_free(pack);
			return -1;
		}
	}
	dev->net_dev->hard_start_xmit(pack, pack->netdev);
	net_packet_free(pack);
	return 0;
}

static int alloc_callback (IF_DEVICE *dev, unsigned int type, ETH_LISTEN_CALLBACK callback)
{
	int i;
	for (i = 0; i < sizeof(dev->cb_info) / sizeof(dev->cb_info[0]); i ++)
	{
		if(0 == dev->cb_info[i].is_busy)
		{
			dev->cb_info[i].is_busy = 1;
			dev->cb_info[i].type = type;
			dev->cb_info[i].func = callback;
			return i;
		}
	}
	return -1;
}


int netif_rx(net_packet *pack) {
	int i;

	IF_DEVICE *dev;
	if((NULL == pack) || (NULL == pack->netdev)) {
		return -1;
	}
	pack->nh.raw = (void *)pack->data + sizeof(machdr);

	for(i = 0; i < INTERFACES_QUANTITY; i ++) {
		if (ifs[i].net_dev == pack->netdev) {
			pack->ifdev = &ifs[i];
			break;
		}
	}

	if (ARP_PROTOCOL_TYPE == pack->protocol) {
		pack->nh.raw = pack->data + sizeof(machdr);
//		packet_dump(pack);
		arp_received_packet(pack);
		net_packet_free(pack);
		return;
	}

	if (IP_PROTOCOL_TYPE == pack->protocol) {
		pack->nh.raw = pack->data + sizeof(machdr);
		pack->h.raw = pack->nh.raw + sizeof(iphdr);
		if (ICMP_PROTO_TYPE == pack->nh.iph->proto) {
//			packet_dump(pack);
			icmp_received_packet(pack);
		}
		if (UDP_PROTO_TYPE == pack->nh.iph->proto) {
			packet_dump(pack);
			udp_received_packet(pack);
		}
	}
	dev = (IF_DEVICE *)pack->ifdev;
	for (i = 0; i < array_len(dev->cb_info); i ++) {
		if(1 == dev->cb_info[i].is_busy) {
			if ((NET_TYPE_ALL_PROTOCOL == dev->cb_info[i].type) ||
			    (dev->cb_info[i].type == pack->protocol)) {
				//may be copy pack for different protocols
				dev->cb_info[i].func(pack);
			}
		}
	}
	net_packet_free(pack);
}

int eth_listen (void *handler, unsigned short type, ETH_LISTEN_CALLBACK callback) {
	IF_DEVICE *dev = (IF_DEVICE *)handler;
	if (NULL == dev) {
		return -1;
	}
	return alloc_callback(dev, type, callback);
}

int find_interface_by_addr(unsigned char ipaddr[4]) {
	int i;
	for (i = 0; i < INTERFACES_QUANTITY; i ++) {
		if(0 == memcmp(ifs[i].ipv4_addr, ipaddr, 4)) {
			return 0;
		}
	}
	return -1;
}

void packet_dump(net_packet *pack) {
	char ip[15], mac[18];
	TRACE("--------dump-----------------\n");
	TRACE("protocol=0x%X\n", pack->protocol);
	TRACE("len=%d\n", pack->len);
	TRACE("mac.mach.type=%d\n", pack->mac.mach->type);
	macaddr_print(mac, pack->mac.mach->src_addr);
	TRACE("mac.mach.src_addr=%s\n", mac);
	macaddr_print(mac, pack->mac.mach->dst_addr);
	TRACE("mac.mach.dst_addr=%s\n", mac);
	if(pack->protocol == ARP_PROTOCOL_TYPE) {
		TRACE("nh.arph.htype=%d\n", pack->nh.arph->htype);
		TRACE("nh.arph.ptype=%d\n", pack->nh.arph->ptype);
		TRACE("nh.arph.hlen=%d\n", pack->nh.arph->hlen);
		TRACE("nh.arph.plen=%d\n", pack->nh.arph->plen);
		TRACE("nh.arph.oper=%d\n", pack->nh.arph->oper);
		macaddr_print(mac, pack->nh.arph->sha);
		TRACE("nh.arph.sha=%s\n", mac);
		ipaddr_print(ip, pack->nh.arph->spa);
		TRACE("nh.arph.spa=%s\n", ip);
		macaddr_print(mac, pack->nh.arph->tha);
		TRACE("nh.arph.tha=%s\n", mac);
		ipaddr_print(ip, pack->nh.arph->tpa);
		TRACE("nh.arph.tpa=%s\n", ip);
	} else if(pack->protocol == IP_PROTOCOL_TYPE) {
    		TRACE("nh.iph.tos=%d\n", pack->nh.iph->tos);
    		TRACE("nh.iph.tot_len=%d\n", pack->nh.iph->tot_len);
    		TRACE("nh.iph.id=%d\n", pack->nh.iph->id);
    		TRACE("nh.iph.frag_off=%d\n", pack->nh.iph->frag_off);
    		TRACE("nh.iph.ttl=%d\n", pack->nh.iph->ttl);
    		TRACE("nh.iph.proto=0x%X\n", pack->nh.iph->proto);
    		TRACE("nh.iph.check=%d\n", pack->nh.iph->check);
    		ipaddr_print(ip, pack->nh.iph->saddr);
    		TRACE("nh.iph.saddr=");
    		ipaddr_print(ip, pack->nh.iph->daddr);
    		TRACE("\nnh.iph.daddr=");
    		if( pack->nh.iph->proto == ICMP_PROTO_TYPE) {
    			TRACE("\nh.icmph.type=%d\n", pack->h.icmph->type);
    			TRACE("h.icmph.code=%d\n", pack->h.icmph->code);
    			TRACE("h.icmph.header_check_summ=%d\n", pack->h.icmph->header_check_summ);
    			TRACE("h.icmph.data=0x%X\n", pack->h.icmph->data);
    		} else if( pack->nh.iph->proto == UDP_PROTO_TYPE) {
    			TRACE("\nh.uh.source=%d\n", pack->h.uh->source);
    			TRACE("h.uh.dest=%d\n", pack->h.uh->dest);
    			TRACE("h.uh.len=%d\n", pack->h.uh->len);
    			TRACE("h.uh.check=%d\n", pack->h.uh->check);
    		}
        }
	TRACE("data=0x%X\n", pack->data);
	TRACE("---------------end-----------------\n");
}
