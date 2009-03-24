/*
 * ping.c
 *
 *  Created on: Mar 20, 2009
 *      Author: anton
 */
#include "types.h"
#include "icmp.h"
#include "shell.h"
#include "ping.h"
#include "net.h"

static int has_responsed;
static void callback(net_packet *pack)
{
	net_packet_free(pack);
	has_responsed = TRUE;
}
int ping (char *eth_name, int cnt, unsigned char *dst)
{
	void *ifdev;
	int i;

	TRACE ("ping from %s...\n", eth_name);
	if (NULL == (ifdev = eth_get_ifdev_by_name(eth_name)))
	{
		TRACE ("Can't find interface %s\n see ifconfig for available intasfaces list\n", eth_name);
		return -1;
	}

	for (i = 0; i < cnt; i ++)
	{
		has_responsed = FALSE;
		TRACE ("ping from ");
		ipaddr_print(eth_get_ipaddr (ifdev), 4);
		TRACE (" to ");
		ipaddr_print(dst, 4);

		icmp_send_echo_request(ifdev, dst, callback);
		sleep (1000);
		if ( FALSE == has_responsed)
		{
			TRACE (" ....timeout\n");
		}
		else TRACE(" ....ok\n");
	}
	icmp_abort_echo_request (ifdev);
}


#define COMMAND_NAME "ping"

static char available_keys[] = {
#include "ping_keys.inc"
};

static void show_help()
{
	printf(
	#include "ping_help.inc"
	);
}

int ping_shell_handler (int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	char *eth_name;
	int cnt;
	unsigned char dst[4];


	keys_amount = parse_arg(COMMAND_NAME, argsc, argsv, available_keys, sizeof(available_keys),
					keys);

	if (keys_amount < 0) {
		show_help();
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}

	if (!get_key('i', keys, keys_amount, &key_value)) {
			printf("Parsing error: chose right eth_name '-i'\n");
			show_help();
			return -1;
	}
	eth_name = key_value;

	if (!get_key('n', keys, keys_amount, &key_value)) {
		cnt = 4;
	}
	else if (1 != sscanf(key_value, "%d", &cnt)) {
		printf("Parsing error: enter validly cnt '-n'\n");
		show_help();
		return -1;
	}

	if (!get_key('d', keys, keys_amount, &key_value)) {
		printf("Parsing error: chose right destination address '-d'\n");
		return -1;
	}
	if (NULL == ipaddr_scan(key_value, dst))
	{
		TRACE ("Error: wrong ip addr format %s\n", key_value);
		return -1;
	}


	ping(eth_name, cnt, dst);
	return 0;
}
