/**                                                         o
 * @file arping.c
 *
 * @brief Ping hosts by ARP requests/replies
 * @date 23.12.09
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <net/net.h>
#include <net/arp.h>
#include <net/etherdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <in.h>
#include <misc.h>

#define COMMAND_NAME     "arping"
#define COMMAND_DESC_MSG "send ARP REQUEST to a neighbour host"
#define HELP_MSG         "Usage: arping [-I if] [-c cnt] host"

static const char *man_page =
	#include "arping_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int cnt     = 4;
	in_device_t *in_dev = inet_dev_find_by_name("eth0");
	struct in_addr dst;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "I:c:h");
	        switch(nextOption) {
	        case 'h':
	                show_help();
	                return 0;
	        case 'I': /* get interface */
                        if (NULL == (in_dev = inet_dev_find_by_name(optarg))) {
                                TRACE("arping: unknown iface %s\n", optarg);
                                return -1;
                        }
                        break;
                case 'c': /* get ping cnt */
            		if (1 != sscanf(optarg, "%d", &cnt)) {
            		        TRACE("arping: bad number of packets to transmit.\n");
            		        return -1;
            		}
            		break;
	        case -1:
	                break;
	        default:
	        	return 0;
	        }
	} while(-1 != nextOption);

	if (argsc == 1) {
		show_help();
		return 0;
	}

        /* get destanation addr */
        if (0 == inet_aton(argsv[argsc - 1], &dst)) {
                LOG_ERROR("wrong ip addr format (%s)\n", argsv[argsc - 1]);
                show_help();
                return -1;
        }
        int cnt_resp = 0;
        char *dst_b = inet_ntoa(dst);
        struct in_addr from;
        from.s_addr = in_dev->ifa_address;
        char *from_b = inet_ntoa(from);
        unsigned char mac[18];
	printf("ARPING %s from %s %s\n", dst_b, from_b, in_dev->dev->name);
	int i, j;
	for(i = 1; i <= cnt; i++) {
		arp_delete_entity(NULL, dst.s_addr, NULL);
		arp_send(ARPOP_REQUEST, ETH_P_ARP, dst.s_addr, in_dev->dev,
		           in_dev->ifa_address, NULL, (in_dev->dev)->dev_addr, NULL);
		usleep(ARP_RES_TIME);
//		if(-1 != (j = arp_lookup(in_dev, dst.s_addr))) {
//			macaddr_print(mac, arp_tables[j].hw_addr);
//			printf("Unicast reply from %s [%s]  %dms\n", dst_b, mac, 0);
//			cnt_resp++;
//		}
	}
	printf("Sent %d probes (%d broadcast(s))\n", cnt, 1);
	printf("Received %d response(s)\n", cnt_resp);
	free(dst_b);
	free(from_b);
	return 0;
}
