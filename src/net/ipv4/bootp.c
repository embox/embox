#include "common.h"
#include "asm/types.h"
#include "net/bootp.h"
#include "net/udp.h"
#include "net/socket.h"
#include "net/in.h"
#include "net/if_ether.h"
#include "net/net_device.h"
#include "net/net_packet.h"
#include "net/net_pack_manager.h"

/*#ifdef DHCP_SUPPORT
static const unsigned char dhcp_cookie[] = {99,130,83,99};
static const unsigned char dhcp_end[] = {255};
static const unsigned char dhcp_discover[] = {53,1,1};
static const unsigned char dhcp_auto[] = {116,1,1};

#ifdef DNS_DHCP
#define MAX_HOST_NAME_SIZE 15
#define DNS_DOMAIN_BUFSIZE 32
char bootp_dns_domain[DNS_DOMAIN_BUFSIZE];

char *ds_host_name = NULL;
static char dns_host_name_all[MAX_HOST_NAME_SIZE+sizeof(__bootp_dns_domain)+1];
static unsigned char dhcp_host_in_name[2+MAX_HOST_NAME_SIZE];
static unsigned char dhcp_fqdn[sizeof(bootp_dns_domain) + 5 + MAX_HOST_NAME_SIZE];
#endif

static const unsigned char dhcp_request[] = {53,1,3};
static const unsigned char dhcp_request_ip[] = {50,4};
static const unsigned char dhcp_param_request_list[] = {55,4,1,15,3,6};
unsigned char dhcp_host_name[7];
typedef enum {
    DHCP_NONE = 0,
    DHCP_DISCOVER,
    DHCP_OFFER,
    DHCP_REQUEST,
    DHCP_ACK
} dhcp_state_t;
static dhcp_state_t dhcp_state = DHCP_NONE;
#endif*/

static struct bootp_header_t* bootp_info;

static ip_addr_t local_ip_addr;

int bootp_discover (void* ifdev)
{
	/*udphdr*/
        int             udp_skt;
	int             srv_skt;
	struct bootp_header_t  b;
	ip_route_t      r;
//	int             retry;
        unsigned long   start;
	unsigned char   *p;
	ip_addr_t       saved_ip_addr;
	struct sockaddr_in addr;
/*#ifdef DHCP_SUPPORT
	int             old_state;
        int             len;
#endif*/
	int             tx_size;
	int             abort = 0;
	static int      xid = SHOULD_BE_RANDOM;
	int             retry = RETRY_COUNT;
	struct _net_device*     dev = (struct _net_device*)ifdev_get_netdevice (ifdev);
	enet_addr_t     enet;
	memcpy (enet, dev->hw_addr, ETH_ALEN);
/*#ifdef HOSTNAME
	int           address;
#endif*/

/*#ifdef DHCP_SUPPORT
	dhcp_state = DHCP_NONE;
#ifdef DNS_DHCP
	memset (dhcp_host_in_name, 0, sizeof (dhcp_host_in_name));
	memset (dhcp_fqdn, 0, sizeof (dhcp_fqdn));
	memset (dns_host_name_all, 0, sizeof (dns_host_name_all));
#endif
#endif*/

//	bootp_info = info;
	// save current local address
	memcpy (saved_ip_addr, local_ip_addr, sizeof (ip_addr_t));

	memset (r.ip, sizeof (ip_addr_t), 0xff);
	memset (r.enet, sizeof (enet_addr_t), 0xff);

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family  = AF_INET;
	addr.sin_port    = htons (PORT_BOOTP_CLIENT);

	udp_skt = socket (AF_INET, SOCK_DGRAM, 0);
	if (udp_skt == -1)
	{
		LOG_ERROR ("bootp socket failed");
		return -1;
	}

	if (bind (udp_skt, (const struct sockaddr*)&addr, sizeof (addr)) == -1)
	{
		LOG_ERROR ("bootp bind failed");
                return -1;
	}

	for (;retry > 0; retry--)
	{
		memset (&b, 0, sizeof (b));
		b.op = BOOTPREQUEST;
		b.htype = HTYPE_ETHERNET;
		b.hlen = 6;
		b.xid = xid++;
		memcpy (b.chaddr, enet, ETH_ALEN);
		memset (local_ip_addr, 0, sizeof (ip_addr_t));

//		srv_skt = socket
//		send 'b' to 'srv_skt'
		net_packet* pack = net_packet_alloc ();
		if (pack == 0)
		{
			LOG_ERROR("bootp net_packet_alloc failed");
			return -1;
		}
		pack->ifdev = ifdev;
		pack->netdev = dev;
		pack->protocol = ETH_P_IP;
		pack->len = sizeof(struct bootp_header_t) + IP_HEADER_SIZE + UDP_HEADER_SIZE + ETH_HEADER_SIZE;
		memset(pack->mac.ethh->dst_addr, 0xff, sizeof(pack->mac.ethh->dst_addr));
	        memcpy(pack->mac.ethh->src_addr, enet, sizeof(pack->mac.ethh->src_addr));

		udphdr uhdr;
		uhdr.source = htons(PORT_BOOTP_CLIENT);
		uhdr.dest = htons (PORT_BOOTP_SERVER);
		uhdr.len = sizeof(struct bootp_header_t);
		uhdr.check = calc_checksumm (&uhdr, UDP_HEADER_SIZE);
		pack->h.uh = &uhdr;

		iphdr ihdr;
		pack->nh.iph = &ihdr;
		ip_addr_t addr;
		memset (addr, 0xff, sizeof (ip_addr_t));
		rebuild_ip_header(pack, 100, AF_INET, 0, sizeof(struct bootp_header_t) + UDP_HEADER_SIZE, enet, addr);

		eth_send (pack);
		net_packet_free (pack);
//
		sleep (100);
		if (!empty_socket (udp_skt))
		{
			if (recv (udp_skt, (void*)&b, sizeof(struct bootp_header_t), 0) != -1)
			{
				if (b.op == BOOTPREPLY)
				{
					if (memcmp (b.chaddr, enet, ETH_ALEN))
					{
						memcpy (&bootp_info, &b, sizeof (struct bootp_header_t));
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

