/*
 * ip_v4.c
 *
 *  Created on: 17.03.2009
 *      Author: sunnya
 */

int ip_received_packet(net_packet *pack)
{
	iphdr *header = pack->nh.iph;
	if (ICMP_PROTOCOL_TYPE == header->proto)
	{
		return;
	}
	//TODO release ip proto
	return;
}
