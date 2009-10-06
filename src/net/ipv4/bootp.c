#include "common.h"
#include "asm/types.h"
#include "net/bootp.h"
#include "net/udp.h"
#include "net/socket.h"
#include "inet/netinet/in.h"
#include "net/if_ether.h"
#include "net/net_device.h"
#include "net/net_packet.h"
#include "net/net_pack_manager.h"
#include "string.h"
#include "conio.h"

#ifdef DHCP_SUPPORT
static const unsigned char dhcp_cookie[] = {99,130,83,99};
static const unsigned char dhcp_end[] = {255};
static const unsigned char dhcp_discover[] = {53,1,1};
static const unsigned char dhcp_auto[] = {116,1,1};

#ifdef DNS_SUPPORT
#define MAX_HOST_NAME_SIZE 15
#define DNS_DOMAIN_BUFSIZE 32
char bootp_dns_domain[DNS_DOMAIN_BUFSIZE];

static char dns_host_name_all[MAX_HOST_NAME_SIZE+sizeof(bootp_dns_domain)+1];
static unsigned char dhcp_host_in_name[2+MAX_HOST_NAME_SIZE];
static unsigned char dhcp_fqdn[sizeof(bootp_dns_domain) + 5 + MAX_HOST_NAME_SIZE];
#endif

static const unsigned char dhcp_request[] = {53,1,3};
static const unsigned char dhcp_request_ip[] = {50,4};
static const unsigned char dhcp_param_request_list[] = {55,4,1,15,3,6};
typedef enum {
    DHCP_NONE = 0,
    DHCP_DISCOVER,
    DHCP_OFFER,
    DHCP_REQUEST,
    DHCP_ACK
} dhcp_state_t;
static dhcp_state_t dhcp_state = DHCP_NONE;

static ip_addr_t bootp_dns_addr = {0};
static int bootp_dns_set = 0;
static int bootp_dns_domain_set = 0;
#endif

static struct _bootp_header_t bootp_info;

const struct _bootp_header_t const* get_bootp_info () {
	return &bootp_info;
}

static ip_addr_t local_ip_addr = {0};
static ip_addr_t local_ip_mask = {0};
static ip_addr_t local_ip_gate = {0};

ip_addr_t* const get_ip_addr () {
	return &local_ip_addr;
}

ip_addr_t* const get_ip_mask () {
        return &local_ip_mask;
}

ip_addr_t* const get_ip_gate () {
        return &local_ip_gate;
}

static unsigned char* add_option (unsigned char* p, unsigned char* opt, int len) {
	if (len == -1) 	{
		len = strlen (opt);
	}
	memcpy (p, opt, len);
	return p + len;
}

int bootp_discover (void* ifdev) {
        int             udp_skt = 0;
	int             srv_skt = 0;
	struct _bootp_header_t  b;
        unsigned long   start = 0;
	ip_addr_t       saved_ip_addr = {0};
	struct sockaddr_in addr;
#ifdef DHCP_SUPPORT
	unsigned char*  p = 0;
	int             old_state = 0;
        int             len = 0;
	int             cont = 0;
#endif
	int             abort = 0;
	static int      xid = SHOULD_BE_RANDOM;
	int             retry = RETRY_COUNT;
	struct _net_device*     dev = (struct _net_device*)ifdev_get_netdevice (ifdev);
	enet_addr_t     enet;
	memcpy (enet, dev->hw_addr, ETH_ALEN);

#ifdef DHCP_SUPPORT
	dhcp_state = DHCP_NONE;
#ifdef DNS_SUPPORT
	memset (dhcp_host_in_name, 0, sizeof (dhcp_host_in_name));
	memset (dhcp_fqdn, 0, sizeof (dhcp_fqdn));
	memset (dns_host_name_all, 0, sizeof (dns_host_name_all));
#endif
#endif

	// save current local address
	memcpy (saved_ip_addr, local_ip_addr, sizeof (ip_addr_t));

	// create socket to DHCP/BOOTP server
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family  = AF_INET;
	addr.sin_port    = htons (PORT_BOOTP_CLIENT);
//	addr.sin_addr.s_addr = 0;

	udp_skt = socket (AF_INET, SOCK_DGRAM, 0);
	if (udp_skt == -1) {
		LOG_ERROR ("bootp socket failed");
		return -1;
	}

	if (bind (udp_skt, (const struct sockaddr*)&addr, sizeof (addr)) == -1) {
		LOG_ERROR ("bootp bind failed");
                return -1;
	}

	memset (&b, 0, sizeof (b));
	TRACE("bootp for(;;) started\n");
	for (;retry > 0; retry--) {
		TRACE("retry #%d\n", retry);
		// fill bootp packet
		b.op = BOOTPREQUEST;
		b.htype = HTYPE_ETHERNET;
		b.hlen = 6;
		b.xid = xid++;
		*(UINT32*)b.options = VM_RFC1048;
		memcpy (b.chaddr, enet, ETH_ALEN);
		memset (local_ip_addr, 0, sizeof (ip_addr_t));

#ifdef DHCP_SUPPORT
		p = b.options;
		switch (dhcp_state) {
			case DHCP_NONE:
			case DHCP_DISCOVER:
				p = add_option (p, (unsigned char*)dhcp_cookie, -1);
				p = add_option (p, (unsigned char*)dhcp_discover, -1);
				p = add_option (p, (unsigned char*)dhcp_auto, -1);
#if defined(DNS_SUPPORT) && defined(HOSTNAME)
				dhcp_host_in_name[0] = TAG_HOST_NAME;
				dhcp_host_in_name[1] = strlen (HOSTNAME);
				memcpy (dhcp_host_in_name + 2, HOSTNAME, strlen (HOSTNAME));
				p = add_option (p, (unsigned char*)dhcp_host_in_name, dhcp_host_in_name[1] + 2);
#endif
				p = add_option (p, (unsigned char*)dhcp_param_request_list, -1);
				dhcp_state = DHCP_DISCOVER;
				break;
			case DHCP_OFFER:
				retry = 0;
				break;
			case DHCP_REQUEST:
				retry = 0;
				b.xid = bootp_info.xid;
				p = add_option (p, (unsigned char*)dhcp_cookie, -1);
				p = add_option (p, (unsigned char*)dhcp_request, -1);
				p = add_option (p, (unsigned char*)dhcp_request_ip, -1);
				p = add_option (p, (unsigned char*)bootp_info.yiaddr, -1);
#if defined(DNS_SUPPORT) && defined(HOSTNAME)
				p = add_option (p, (unsigned char*)dhcp_host_in_name, 2 + dhcp_host_in_name[1]);
				memset (dhcp_fqdn, 0, sizeof (dhcp_fqdn));
				dhcp_fqdn[0] = TAG_FQDN;
				strcpy (dns_host_name_all, HOSTNAME);
				strcat (dns_host_name_all, ".");
				strcat (dns_host_name_all, (char*)bootp_dns_domain);
				dhcp_fqdn[1] = 3 + strlen (dns_host_name_all);
				memcpy (dhcp_fqdn + 5, dns_host_name_all, strlen (dns_host_name_all));
				p = add_option (p, (unsigned char*)dhcp_fqdn, dhcp_fqdn[1] + 2);
#endif
				p = add_option (p, (unsigned char*)dhcp_param_request_list, -1);
				p = add_option (p, (unsigned char*)dhcp_end, -1);
				dhcp_state = DHCP_REQUEST;
				memset (b.yiaddr, 0, 4);
				memset (b.siaddr, 0, 4);


		}
		old_state = dhcp_state;
#endif

		// create 'raw' packet
		TRACE("create raw packet\n");
		net_packet* pack = net_packet_alloc ();
		if (pack == 0) {
			LOG_ERROR("bootp net_packet_alloc failed");
			return -1;
		}
		pack->ifdev = ifdev;
		pack->netdev = dev;
		pack->protocol = ETH_P_IP;
		pack->len = BOOTP_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE + ETH_HEADER_SIZE;
		pack->mac.ethh = (struct _ethhdr*)pack->data;
		memset(pack->mac.ethh->dst_addr, 0xff, sizeof(pack->mac.ethh->dst_addr));
	        memcpy(pack->mac.ethh->src_addr, enet, sizeof(pack->mac.ethh->src_addr));
		pack->mac.ethh->type = ETH_P_IP;

		memcpy (pack->data + IP_HEADER_SIZE + UDP_HEADER_SIZE + ETH_HEADER_SIZE, &b, BOOTP_HEADER_SIZE);

                pack->nh.iph = (struct _iphdr*)(pack->data + ETH_HEADER_SIZE);
                ip_addr_t daddr;
		ip_addr_t saddr;
                memset (saddr, 0, sizeof (ip_addr_t));
                memset (daddr, 0xff, sizeof (ip_addr_t));
                rebuild_ip_header(pack, 100, /*AF_INET*/0x11, 0, BOOTP_HEADER_SIZE + UDP_HEADER_SIZE + IP_HEADER_SIZE, saddr, daddr);

		pack->h.uh = (struct _udphdr*)(pack->data + ETH_HEADER_SIZE + IP_HEADER_SIZE);
		pack->h.uh->source = htons(PORT_BOOTP_CLIENT);
		pack->h.uh->dest = htons (PORT_BOOTP_SERVER);
		pack->h.uh->len = BOOTP_HEADER_SIZE + UDP_HEADER_SIZE;
		pack->h.uh->check = 0;
//		pack->h.uh->check = calc_checksumm (pack->h.uh, BOOTP_HEADER_SIZE + UDP_HEADER_SIZE);

		arp_add_entity (ifdev, daddr, pack->mac.ethh->dst_addr);
		TRACE("eth_send\n");
		eth_send (pack);
		net_packet_free (pack);

		// wait a bit
		sleep (1000);

		// try to read reply
		if (!empty_socket (udp_skt)) {
			TRACE("socket not empty\n");
			if (recv (udp_skt, (void*)&b, sizeof(struct _bootp_header_t), 0) != -1) {
				if (b.op == BOOTPREPLY) {
					if (memcmp (b.chaddr, enet, ETH_ALEN)) {
#ifdef DHCP_SUPPORT
						p = b.options;
						if (!memcmp (p, dhcp_cookie, sizeof (dhcp_cookie))) {
							// find the DHCP message tag
							while (*p != TAG_DHCP_MESS_TYPE) {
								p += p[1] + 2;
								if (p >= b.options + BOOTP_HEADER_SIZE) {
									cont = 1;
								}
							}
							if (cont) continue;

							p += 2;
							switch (dhcp_state) {
								case DHCP_DISCOVER:
									if (*p == DHCP_MESS_TYPE_OFFER) {
										if (!memcmp (b.chaddr, enet, 6)) {
											old_state = dhcp_state;
											dhcp_state = DHCP_REQUEST;
											p = b.options + 4;
											unsigned char* end = b.options + BOOTP_HEADER_SIZE;
											int optlen = 0;
											while (p < end) {
												unsigned char* tag = p;
												if (*tag == TAG_END) break;
												if (*tag == TAG_PAD) optlen = 1;
												else {
													optlen = p[1];
													p += 2;
													switch (*tag) {
#ifdef DNS_SUPPORT
														case TAG_DOMAIN_SERVER:
															memcpy(&bootp_dns_addr, p, 4);
															bootp_dns_set = 1;
														break;
														case TAG_DOMAIN_NAME:
															if (optlen < sizeof(bootp_dns_domain)) {
																memcpy (bootp_dns_domain, p, optlen);
																bootp_dns_domain[optlen] = 0;
																bootp_dns_domain_set = 1;
															} else {
																LOG_ERROR("DNS domain is too long.");
															}
														break;
#endif
													}
												}
												p += optlen;
											}
										}
									}
									break;
                                                                case DHCP_REQUEST:
                                                                        if (*p == DHCP_MESS_TYPE_OFFER) {
                                                                                if (!memcmp (b.chaddr, enet, 6)) {
											memcpy (local_ip_addr, b.yiaddr, 4);
											memcpy (local_ip_gate, b.giaddr, 4);
											p = b.options + 4;
                                                                                        unsigned char* end = b.options + BOOTP_HEADER_SIZE;
                                                                                        int optlen = 0;
											while (p < end) {
												unsigned char* tag = p;
                                                                                                if (*tag == TAG_END) break;
                                                                                                if (*tag == TAG_PAD) optlen = 1;
												else {
													optlen = p[1];
                                                                                                        p += 2;
                                                                                                        switch (*tag) {
														case TAG_SUBNET_MASK:
															memcpy(local_ip_mask, p, 4);
															break;
														case TAG_GATEWAY:
															memcpy(local_ip_gate, p, 4);
															break;
													}
												}
												p += optlen;
											}
										}
										memcpy (&bootp_info, &b, sizeof (struct _bootp_header_t));
										return 0;
                                                                        }
                                                                        break;

							}
							if (*p == DHCP_MESS_TYPE_NAK)
							{
								dhcp_state = DHCP_NONE;
							}
						}
#else
						memcpy (&bootp_info, &b, sizeof (struct _bootp_header_t));
						memcpy (local_ip_addr, &b.yiaddr, 4);
						memcpy (local_ip_gate, &b.giaddr, 4);
#endif
						return 1;
					}
				}
			}
		}
	}

	//there no bootp info, so restore ip address
	memcpy (local_ip_addr, saved_ip_addr, sizeof (ip_addr_t));
	return 0;
}

#define BUG_LIBC
#ifdef BUG_LIBC
char* strcat (char* s1, const char* s2) {
	if (!s1 || !s2) return NULL;
	char* s = s1;
	while (*s1) s1++;
	strcpy (s1, s2);
	return s;
}
#endif

