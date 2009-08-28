/**
 * \file ping.c
 *
 * \date Mar 20, 2009
 * \author anton
 */
#include "asm/types.h"
#include "conio/conio.h"
#include "net/icmp.h"
#include "shell.h"
#include "ping.h"
#include "net/net.h"
#include "net/eth.h"
#include "net/if_device.h"
#include "shell_command.h"

#define COMMAND_NAME "ping"
#define COMMAND_DESC_MSG "send ICMP ECHO_REQUEST to network hosts"
static const char *help_msg =
	#include "ping_help.inc"
	;
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG);



static char available_keys[] = {
#include "ping_keys.inc"
		};

static void show_help() {
	printf(
#include "ping_help.inc"
	);
}

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
/**
 * handler of command is named "ping"
 * @param argsc - quantity of params
 * @param argsv - list of arguments
 */
static int exec(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	//	char *eth_name;
	int cnt;
	int timeout;
	int ttl;
	unsigned char dst[4];
	void *ifdev;

	keys_amount = parse_arg(COMMAND_NAME, argsc, argsv, available_keys,
			sizeof(available_keys), keys);

	if (keys_amount < 0) {
		LOG_ERROR("during parsing params\n");
		show_help();
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}

	//get interface
	if (!get_key('I', keys, keys_amount, &key_value)) {
//		LOG_ERROR("choose right interface name '-i'\n");
//		show_help();
//		return -1;
		ifdev = ifdev_find_by_name("eth0");
	} else if (NULL == (ifdev = ifdev_find_by_name(key_value))) {
		LOG_ERROR("Can't find interface %s\n see ifconfig for available interfaces list\n", key_value);
		show_help();
		return -1;
	}
	//get destanation addr
	if (!get_key('d', keys, keys_amount, &key_value)) {
		LOG_ERROR("you must choose right destination address '-d'\n");
		show_help();
		return -1;
	} else if (NULL == ipaddr_scan(key_value, dst)) {
		LOG_ERROR("wrong ip addr format (%s)\n", key_value);
		show_help();
		return -1;
	}

	//get ping cnt
	if (!get_key('c', keys, keys_amount, &key_value)) {
		cnt = 10;
	} else if (1 != sscanf(key_value, "%d", &cnt)) {
		LOG_ERROR("enter validly cnt '-c'\n");
		show_help();
		return -1;
	}

        //get icmp ttl
        if (!get_key('t', keys, keys_amount, &key_value)) {
                ttl = 64;
        } else if (1 != sscanf(key_value, "%d", &ttl)) {
                LOG_ERROR("enter validly ttl '-t'\n");
                show_help();
                return -1;
        }

        //get ping timeout
        if (!get_key('W', keys, keys_amount, &key_value)) {
                timeout = 1000;
        } else if (1 != sscanf(key_value, "%d", &timeout)) {
                LOG_ERROR("enter validly timeout '-W'\n");
                show_help();
                return -1;
        }

	//carry out command
	ping(ifdev, dst, cnt, timeout, ttl);
	return 0;
}
