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
#define HELP_MSG         "Usage: ping [-I if] [-c cnt] [-W timeout] [-t ttl] host"
static const char *man_page =
	#include "ping_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int has_responsed;
static void callback(struct sk_buff *pack) {
	kfree_skb(pack);
	has_responsed = true;
}

static int ping(void *ifdev, struct in_addr dst, int cnt, int timeout, int ttl, int quiet) {
	printf("PING to %s\n", inet_ntoa(dst));

	int cnt_resp = 0, cnt_err = 0;

	if (0 == cnt)
		return 0;

	while (1) {
		if ((0 <= cnt) && !(cnt--))
			break;

		has_responsed = false;
		struct in_addr from;
		from.s_addr = inet_dev_get_ipaddr(ifdev);
		if(!quiet) printf("from %s", inet_ntoa(from));
		if(!quiet) printf(" to %s", inet_ntoa(dst));
		if(!quiet) printf(" ttl=%d ", ttl);
		icmp_send_echo_request(ifdev, dst.s_addr, ttl, callback);
		usleep(timeout);
		if (false == has_responsed) {
			if(!quiet) printf(" ....timeout\n");
			icmp_abort_echo_request(ifdev);
			cnt_err++;
		} else {
			if(!quiet) printf(" ....ok\n");
			cnt_resp++;
		}
	}
	printf("--- %s ping statistics ---\n", inet_ntoa(dst));
	printf("%d packets transmitted, %d received, %d%% packet loss", cnt_resp+cnt_err, cnt_resp, cnt_err*100/(cnt_err+cnt_resp));
	icmp_abort_echo_request(ifdev);
	return 0;
}

static int exec(int argsc, char **argsv) {
	int cnt     = 4;
	int timeout = 1000;
	int ttl     = 64;
	int quiet   = 0;
	void *ifdev = inet_dev_find_by_name("eth0");
	struct in_addr dst;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "qI:c:t:W:h");
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
	ping(ifdev, dst, cnt, timeout, ttl, quiet);
	return 0;
}
