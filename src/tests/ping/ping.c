/**
 * \file ping.c
 *
 * \date Mar 20, 2009
 * \author: anton
 */
#include "types.h"
#include "icmp.h"
#include "shell.h"
#include "ping.h"
#include "net.h"

#define COMMAND_NAME "ping"

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

static int ping(void *ifdev, unsigned char *dst, int cnt) {
	int cnt_resp, cnt_err;

	if (0 == cnt)
		return 0;

	while (1) {
		if ((0 <= cnt) && (cnt--))
			break;

		has_responsed = FALSE;
		TRACE ("ping from ");
		ipaddr_print(eth_get_ipaddr(ifdev), 4);
		TRACE (" to ");
		ipaddr_print(dst, 4);

		icmp_send_echo_request(ifdev, dst, callback);
		sleep(1000);
		if (FALSE == has_responsed) {
			TRACE (" ....timeout\n");
			cnt_resp++;
		} else {
			TRACE(" ....ok\n");
			cnt_err++;
		}
	}
	TRACE ("good answer = %d\n missed packet = %d\n", cnt_resp, cnt_err);
	icmp_abort_echo_request(ifdev);
	return 0;
}
/**
 * handler of command is named "ping"
 * @param argsc - quantity of params
 * @param argsv - list of arguments
 */
int ping_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	//	char *eth_name;
	int cnt;
	unsigned char dst[4];
	void *ifdev;

	keys_amount = parse_arg(COMMAND_NAME, argsc, argsv, available_keys,
			sizeof(available_keys), keys);

	if (keys_amount < 0) {
		TRACE("ERROR: during parsing params\n");
		show_help();
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}

	//get interface
	if (!get_key('i', keys, keys_amount, &key_value)) {
		printf("ERROR: choose right interface name '-i'\n");
		show_help();
		return -1;
	}
	if (NULL == (ifdev = eth_get_ifdev_by_name(key_value))) {
		TRACE ("ERROR: Can't find interface %s\n see ifconfig for available interfaces list\n", key_value);
		show_help();
		return -1;
	}
	//get destanation addr
	if (!get_key('d', keys, keys_amount, &key_value)) {
		printf("ERROR: you must choose right destination address '-d'\n");
		show_help();
		return -1;
	}
	if (NULL == ipaddr_scan(key_value, dst)) {
		TRACE ("ERROR: wrong ip addr format (%s)\n", key_value);
		show_help();
		return -1;
	}

	//get ping cnt
	if (!get_key('c', keys, keys_amount, &key_value)) {
		cnt = -1;
	} else if (1 != sscanf(key_value, "%d", &cnt)) {
		printf("ERROR: enter validly cnt '-c'\n");
		show_help();
		return -1;
	}

	//carry out command
	ping(ifdev, dst, cnt);
	return 0;
}
