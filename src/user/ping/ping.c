/**
 * \file ping.c
 *
 * \date Mar 20, 2009
 * \author anton
 */
#include "shell_command.h"
#include "net/icmp.h"
#include "net/net.h"
#include "net/etherdevice.h"
#include "net/inetdevice.h"
#include "net/skbuff.h"
#include "misc.h"

#define COMMAND_NAME     "ping"
#define COMMAND_DESC_MSG "send ICMP ECHO_REQUEST to network hosts"
#define HELP_MSG         "Usage: ping [-I if] [-c cnt] [-W timeout] [-t ttl] \n\
       [-i interval] [-p pattern] [-s packetsize] host"

static const char *man_page =
	#include "ping_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int has_responsed;

static void callback(struct sk_buff *pack) {
	kfree_skb(pack);
	has_responsed = true;
}

static int ping(in_device_t *ifdev, struct in_addr dst, int cnt, int timeout, int ttl,
	    int quiet, unsigned packsize, int interval, unsigned short pattern) {
	char *dst_b = inet_ntoa(dst);
	printf("PING %s %d bytes of data.\n", dst_b, packsize);

	int cnt_resp = 0, cnt_err = 0;

	if (0 == cnt) {
		return 0;
	}

	struct in_addr from;
	from.s_addr = inet_dev_get_ipaddr(ifdev);
	char *from_b = inet_ntoa(from);

	int i;
	for(i = 1; i <= cnt; i++) {
		has_responsed = false;
		if(!quiet) printf("%d bytes from %s", packsize, from_b);

		if(!quiet) printf(" to %s:", dst_b);
		icmp_send_echo_request(ifdev, dst.s_addr, ttl, callback,
						packsize, pattern, i);
		usleep(timeout);
		if (false == has_responsed) {
			if(!quiet) printf(" Destination Host Unreachable\n");
			icmp_abort_echo_request(ifdev);
			cnt_err++;
		} else {
			if(!quiet) printf(" icmp_seq=%d ttl=%d time=%d ms\n",
						i, ttl, /*TODO*/0);
			//if(!quiet) printf(" ....ok\n");
			cnt_resp++;
		}
		usleep(interval);
	}
	printf("--- %s ping statistics ---\n", inet_ntoa(dst));
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		cnt_resp+cnt_err, cnt_resp, cnt_err*100/(cnt_err+cnt_resp), 0);
	//printf("rtt min/avg/max/mdev = %f/%f/%f/%f ms", 0, 0, 0, 0);
	icmp_abort_echo_request(ifdev);
	free(dst_b);
	free(from_b);
	return 0;
}

static int exec(int argsc, char **argsv) {
	int cnt     = 4;
	unsigned packsize = 0x38;
	int timeout = 1;
	int interval = 0;
	int pattern = 0xff;
	int ttl     = 64;
	int quiet   = 0;
	in_device_t *ifdev = inet_dev_find_by_name("eth0");
	struct in_addr dst;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "qI:c:t:W:s:i:p:h");
	        switch(nextOption) {
	        case 'h':
	                show_help();
	                return 0;
	        case 'q':
	    		quiet = 1;
	    		break;
	        case 'I': /* get interface */
                        if (NULL == (ifdev = inet_dev_find_by_name(optarg))) {
                                TRACE("ping: unknown iface %s\n", optarg);
                                return -1;
                        }
                        break;
                case 'c': //get ping cnt */
            		if (1 != sscanf(optarg, "%d", &cnt)) {
            		        TRACE("ping: bad number of packets to transmit.\n");
            		        return -1;
            		}
            		break;
            	case 't': /* get icmp ttl */
            		if (1 != sscanf(optarg, "%d", &ttl)) {
            			TRACE("ping: can't set unicast time-to-live: Invalid argument\n");
            		        return -1;
            		}
            		break;
            	case 'W': /* get ping timeout */
            		sscanf(optarg, "%d", &timeout);
            		break;
            	case 's': /* get packet size */
            		sscanf(optarg, "%d", &packsize);
            		break;
            	case 'i': /* get interval */
            		sscanf(optarg, "%d", &interval);
            		break;
            	case 'p': /* get pattern */
            		sscanf(optarg, "%d", &pattern);
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
	//carry out command
	ping(ifdev, dst, cnt, timeout*1000, ttl, quiet, packsize,
						    interval*1000, (__u16)pattern);
	return 0;
}
