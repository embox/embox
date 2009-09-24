/**
 * \file ping.c
 *
 * \date Mar 20, 2009
 * \author anton
 */
#include "shell_command.h"
#include "net/icmp.h"
#include "net/net.h"
#include "net/eth.h"
#include "net/if_device.h"

#define COMMAND_NAME "ping"
#define COMMAND_DESC_MSG "send ICMP ECHO_REQUEST to network hosts"
static const char *help_msg =
	#include "ping_help.inc"
	;
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG);

static int has_responsed;
static void callback(net_packet *pack) {
	net_packet_free(pack);
	has_responsed = TRUE;
}

static int ping(void *ifdev, unsigned char *dst, int cnt, int timeout, int ttl) {
	char ip[15];
	ipaddr_print(ip, dst);
	printf("PING to %s\n", ip);

	int cnt_resp = 0, cnt_err = 0;

	if (0 == cnt)
		return 0;

	while (1) {
		if ((0 <= cnt) && !(cnt--))
			break;

		has_responsed = FALSE;
		ipaddr_print(ip, ifdev_get_ipaddr(ifdev));
		printf("from %s", ip);
		ipaddr_print(ip, dst);
		printf(" to %s", ip);
		printf(" ttl=%d ", ttl);
		icmp_send_echo_request(ifdev, dst, ttl, callback);
		sleep(timeout);
		if (FALSE == has_responsed) {
			printf(" ....timeout\n");
			icmp_abort_echo_request(ifdev);
			cnt_err++;
		} else {
			printf(" ....ok\n");
			cnt_resp++;
		}
	}
	printf("--- %d.%d.%d.%d ping statistics ---\n", dst[0], dst[1], dst[2], dst[3]);
	printf("%d packets transmitted, %d received, %d%% packet loss", cnt_resp+cnt_err, cnt_resp, cnt_err*100/(cnt_err+cnt_resp));
	icmp_abort_echo_request(ifdev);
	return 0;
}

static int exec(int argsc, char **argsv) {
	int cnt     = 4;
	int timeout = 1000;
	int ttl     = 64;
	void *ifdev = ifdev_find_by_name("eth0");
	unsigned char dst[4];
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "I:c:t:W:h");
	        switch(nextOption) {
	        case 'h':
	                show_help();
	                return 0;
	        case 'I': /* get interface */
                        if (NULL == (ifdev = ifdev_find_by_name(optarg))) {
                                LOG_ERROR("Can't find interface %s\n see ifconfig for available interfaces list\n", optarg);
                                show_help();
                                return -1;
                        }
                        break;
                case 'c': //get ping cnt */
            		if (1 != sscanf(optarg, "%d", &cnt)) {
            		        LOG_ERROR("enter validly cnt '-c'\n");
            		        show_help();
            		        return -1;
            		}
            		break;
            	case 't': /* get icmp ttl */
            		if (1 != sscanf(optarg, "%d", &ttl)) {
            		        LOG_ERROR("enter validly ttl '-t'\n");
            		        show_help();
            		        return -1;
            		}
            		break;
            	case 'W': /* get ping timeout */
            		if (1 != sscanf(optarg, "%d", &timeout)) {
            		        LOG_ERROR("enter validly timeout '-W'\n");
            		        show_help();
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
        if (NULL == ipaddr_scan(argsv[argsc - 1], dst)) {
                LOG_ERROR("wrong ip addr format (%s)\n", argsv[argsc - 1]);
                show_help();
                return -1;
        }
	//carry out command
	ping(ifdev, dst, cnt, timeout, ttl);
	return 0;
}
