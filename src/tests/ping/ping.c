/*
 * ping.c
 *
 *  Created on: Mar 20, 2009
 *      Author: anton
 */
#include "types.h"
#include "icmp.h"
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

	if (NULL == (ifdev = eth_get_if(eth_name)))
		return -1;

	for (i = 0; i < cnt; i ++)
	{
		has_responsed = FALSE;
		icmp_send_echo_request(ifdev, dst, callback);
		sleep (1000);
		if ( FALSE == has_responsed)
		{
			TRACE ("timeout");
		}
	}
	icmp_abort_echo_request (ifdev);
}
